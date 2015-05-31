/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/rpc.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <tuple>

#include "dharc/rpc_commands.hpp"
#include "dharc/node.hpp"
#include "dharc/fabric.hpp"
#include "dharc/harc.hpp"
#include "dharc/parse.hpp"
#include "dharc/rpc_packer.hpp"

using dharc::rpc::Packer;
using std::cout;
using std::istream;
using std::ostream;
using std::string;
using std::vector;
using std::list;
using dharc::Node;
using dharc::Harc;
using dharc::fabric;
using dharc::rpc::Command;
using dharc::parser::Context;
using dharc::parser::noact;
using dharc::parser::value;

namespace {

/* rpc::Command::nop */
bool rpc_nop() {
	return false;
}

/* rpc::Command::version */
int rpc_version() {
	return static_cast<int>(Command::end);
}

/* rpc::Command::query */
Node rpc_query(const Node &n1, const Node &n2) {
	return fabric.query({n1, n2});
}

/* rpc::Command::define_const */
bool rpc_define_const(const Node &n1, const Node &n2, const Node &h) {
	fabric.define({n1, n2}, h);
	return true;
}

/* rpc::Command::define */
bool rpc_define(
		const Node &n1,
		const Node &n2,
		const vector<vector<Node>> &p) {
	fabric.define({n1, n2}, p);
	return true;
}

/* rpc::Command::partners */
list<Node> rpc_partners(const Node &n) {
	const list<Harc*> &part = fabric.partners(n);
	list<Node> res;
	for (auto i : part) {
		res.push_back(i->tailPartner(n));
	}
	return res;
}

/* rpc::Command::unique */
Node rpc_unique() {
	// return Node::unique();
	return dharc::null_n;
}

size_t rpc_linkcount() {
	return fabric.linkCount();
}

size_t rpc_nodecount() {
	return fabric.nodeCount();
}

float rpc_changes() {
	return fabric.changesPerSecond();
}

float rpc_queries() {
	return fabric.queriesPerSecond();
}

/* Register the handler for each rpc command */
dharc::rpc::commands_t commands {
	rpc_nop,
	rpc_version,
	rpc_query,
	rpc_define_const,
	rpc_define,
	rpc_partners,
	rpc_unique,
	rpc_linkcount,
	rpc_nodecount,
	rpc_changes,
	rpc_queries
};
};  // namespace

/* ==== DO NOT EDIT ========================================================= */

namespace {
template<typename Ret>
Ret unpack(std::istream &is) {
	Ret res = Packer<Ret>::unpack(is);
	if (is.peek() == ',') is.ignore();
	return res;
}

template<int...> struct seq {};
template<int N, int... S> struct gens : gens<N-1, N-1, S...> {};
template<int... S> struct gens<0, S...>{ typedef seq<S...> type; };

template<typename Ret, typename T, typename F, int... S>
Ret callFunc(seq<S...>, T &params, F f) {
	return f(std::get<S>(params)...);
}

template <typename Ret, typename... Args>
void execute(std::istream &is, std::ostream &os, Ret(*f)(Args ...args)) {
	std::tuple<typename std::decay<Args>::type...> params {
		unpack<typename std::decay<Args>::type>(is)... };
	 Packer<Ret>::pack(os,
		callFunc<Ret>(typename gens<sizeof...(Args)>::type(), params, f));
}

/*
 * Template nested if to find correct function to call for the received
 * RPC command.
 */
template<int S>
void callCmd(istream &is, ostream &os, int cmd) {
	if (cmd == S) {
		execute(is, os, std::get<S>(commands));
	} else {
		callCmd<S + 1>(is, os, cmd);
	}
}

/* Base case, do nothing */
template<>
inline void callCmd<static_cast<int>(Command::end)>(
	istream &is,
	ostream &os,
	int cmd) {}

};  // namespace

/* ========================================================================== */

void dharc::rpc::process_msg(istream &is, ostream &os) {
	Context parse(is);
	int cmd = 0;
	if (parse("{\"c\": ", value<int>{cmd}, ", \"args\": [", noact)) {
		if (cmd >= static_cast<int>(Command::end) || cmd < 0) cmd = 0;
		callCmd<0>(is, os, cmd);
	}
}

