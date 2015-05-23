/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/arch.hpp"

#include <vector>

using dharc::Nid;
using dharc::rpc::Command;

void dharc::start(int argc, char *argv[]) {
	dharc::rpc::connect("tcp://localhost:7878");
}

void dharc::stop() {
}

Nid dharc::query(const Nid &a, const Nid &b) {
	return dharc::rpc::send<Nid>(Command::query, a, b);
}

void dharc::define(const Nid &a, const Nid &b, const Nid &h) {
	dharc::rpc::send<Nid>(Command::define_const, a, b, h);
}

void dharc::define(const Nid &a, const Nid &b, const vector<vector<Nid>> &p) {
	dharc::rpc::send<Nid>(Command::define, a, b, p);
}

