/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/monitor.hpp"

#include <vector>
#include <list>
#include <iostream>
#include <string>

#include "dharc/node.hpp"
#include "dharc/tail.hpp"

using dharc::Node;
using dharc::Tail;
using dharc::Rpc;
using dharc::Monitor;
using std::string;
using std::cout;
using std::vector;
using std::list;


Monitor::Monitor(const char *host, int port) : Rpc(host, port) {
}



Monitor::~Monitor() {
	// Disconnect RPC
}



Node Monitor::unique() {
	return send<Command::unique>();
}



Node Monitor::query(const Node &a, const Node &b) {
	return send<Command::query>(a, b);
}



void Monitor::define(const Node &a, const Node &b, const Node &h) {
	send<Command::define_const>(a, b, h);
}



void Monitor::define(const Node &a,
					const Node &b,
					const vector<Node> &p) {
	send<Command::define>(a, b, p);
}



vector<Tail> Monitor::partners(const Node &n, int count) {
	return send<Command::partners>(n, count);
}



size_t Monitor::linkCount() {
	return send<Command::linkcount>();
}



size_t Monitor::nodeCount() {
	return send<Command::linkcount>();
}



float Monitor::queriesPerSecond() {
	return send<Command::queries>();
}



float Monitor::changesPerSecond() {
	return send<Command::changes>();
}

