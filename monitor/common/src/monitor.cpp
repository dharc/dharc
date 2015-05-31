/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/monitor.hpp"

#include <vector>
#include <list>
#include <iostream>
#include <string>

#include "dharc/node.hpp"

using dharc::Node;
using dharc::rpc::Command;
using dharc::rpc::send;
using dharc::Monitor;
using std::string;
using std::cout;
using std::vector;
using std::list;


Monitor::Monitor(const char *host, int port) {
	uri_ = "tcp://";
	uri_ += host;
	uri_ += ':';
	uri_ += std::to_string(port);
	dharc::rpc::connect(uri_.c_str());

	// Do a version check!
	if (send<Command::version>() != static_cast<int>(Command::end)) {
		cout << "!!! dharcd uses different version of rpc protocol !!!";
		cout << std::endl;
	}
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
					const vector<vector<Node>> &p) {
	send<Command::define>(a, b, p);
}



list<Node> Monitor::partners(const Node &n) {
	return send<Command::partners>(n);
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
