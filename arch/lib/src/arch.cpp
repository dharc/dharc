/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/arch.hpp"

#include <vector>
#include <list>
#include <iostream>

#include "dharc/nid.hpp"

using dharc::Nid;
using dharc::rpc::Command;
using dharc::rpc::send;

void dharc::start(int argc, char *argv[]) {
	dharc::rpc::connect("tcp://localhost:7878");

	// Do a version check!
	if (send<Command::version>() != static_cast<int>(Command::end)) {
		std::cout << "!!! dharcd uses different version of rpc protocol !!!";
		std::cout << std::endl;
	}
}

void dharc::stop() {
}

Nid dharc::query(const Nid &a, const Nid &b) {
	return send<Command::query>(a, b);
}

void dharc::define(const Nid &a, const Nid &b, const Nid &h) {
	send<Command::define_const>(a, b, h);
}

void dharc::define(const Nid &a, const Nid &b, const vector<vector<Nid>> &p) {
	send<Command::define>(a, b, p);
}

list<Nid> dharc::partners(const Nid &n) {
	return send<Command::partners>(n);
}

