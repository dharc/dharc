/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/rpc.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "dharc/rpc_commands.hpp"
#include "dharc/nid.hpp"
#include "dharc/fabric.hpp"

using std::cout;
using std::istream;
using std::ostream;
using std::string;
using std::vector;
using dharc::Nid;
using dharc::fabric;
using dharc::rpc::Command;

static bool rpc_nop() {
	return false;
}

static int rpc_version() {
	return 1;
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

dharc::rpc::commands_t commands {
	rpc_nop,
	rpc_version,
	rpc_query,
	rpc_define_const,
	rpc_define
};

/* ========================================================================== */

static Command get_cmd(istream &is) {
	int cmdtmp;
	string str;
	std::getline(is, str, ' ');
	if (str != "{\"c\":") return Command::nop;
	is >> cmdtmp;
	std::getline(is, str, '[');
	if (str != ", \"args\": ") return Command::nop;
	return static_cast<Command>(cmdtmp);
}

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
	Command cmd = get_cmd(is);
	callCmd<0>(is, os, cmd);
}

