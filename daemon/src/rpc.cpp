/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/rpc.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "dharc/rpc_commands.hpp"
#include "dharc/nid.hpp"
#include "dharc/fabric.hpp"
#include "dharc/harc.hpp"
#include "dharc/parse.hpp"

using std::cout;
using std::istream;
using std::ostream;
using std::string;
using std::vector;
using std::list;
using dharc::Nid;
using dharc::Harc;
using dharc::fabric;
using dharc::rpc::Command;

static bool rpc_nop() {
	return false;
}

static int rpc_version() {
	return static_cast<int>(Command::end);
}

static Nid rpc_query(const Nid &n1, const Nid &n2) {
	cout << "Query: " << n1 << ", " << n2 << std::endl;
	return fabric.get(n1, n2).query();
}

static bool rpc_define_const(const Nid &n1, const Nid &n2, const Nid &h) {
	cout << "Define: " << n1 << ", " << n2 << " -> " << h << std::endl;
	fabric.get(n1, n2).define(h);
	return true;
}

static bool rpc_define(
		const Nid &n1,
		const Nid &n2,
		const vector<vector<Nid>> &p) {
	cout << "Define a path" << std::endl;
	fabric.get(n1, n2).define(p);
	return true;
}

static list<Nid> rpc_partners(const Nid &n) {
	const list<Harc*> &part = fabric.partners(n);
	list<Nid> res;
	for (auto i : part) {
		res.push_back(i->tail_partner(n));
	}
	return res;
}

static Nid rpc_unique() {
	return Nid::unique();
}

dharc::rpc::commands_t commands {
	rpc_nop,
	rpc_version,
	rpc_query,
	rpc_define_const,
	rpc_define,
	rpc_partners,
	rpc_unique
};

/* ========================================================================== */

/*
 * Template nested if to find correct function to call for the received
 * RPC command.
 */
template<int S>
void callCmd(istream &is, ostream &os, Command cmd) {
	if (static_cast<int>(cmd) == S) {
		dharc::rpc::execute(is, os, std::get<S>(commands));
	} else {
		callCmd<S + 1>(is, os, cmd);
	}
}

/* Base case, do nothing */
template<>
void callCmd<static_cast<int>(Command::end)>(
	istream &is,
	ostream &os,
	Command cmd) {}

void dharc::rpc::process_msg(istream &is, ostream &os) {
	int cmdtmp;

	if (parse(is, "{\"c\": ", Token<int>{cmdtmp}, ", \"args\": [")) {
		Command cmd = static_cast<Command>(cmdtmp);
		callCmd<0>(is, os, cmd);
	}
}

