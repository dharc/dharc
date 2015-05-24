/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/rpc.hpp"

#include <iostream>

#include "dharc/rpc_commands.hpp"
#include "dharc/nid.hpp"
#include "dharc/fabric.hpp"

using std::cout;
using std::istream;
using std::ostream;
using dharc::Nid;
using dharc::fabric;

Nid query_command(const Nid &n1, const Nid &n2) {
	cout << "Query: " << n1 << ", " << n2 << std::endl;
	return fabric.get(n1, n2).query();
}

bool define_const_command(const Nid &n1, const Nid &n2, const Nid &h) {
	cout << "Define: " << n1 << ", " << n2 << " -> " << h << std::endl;
	fabric.get(n1, n2).define(h);
	return true;
}

#define E(C) execute(is, os, C); break

void dharc::rpc::process_msg(istream &is, ostream &os) {
	int cmdtmp;
	Command cmd;
	is.ignore(6);  // {"c":
	is >> cmdtmp;
	cmd = static_cast<Command>(cmdtmp);
	is.ignore(100, '[');  // , "args": [

	switch(cmd) {
	case Command::query:		E(query_command);
	case Command::define_const:	E(define_const_command);
	default: break;
	}
}

