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


size_t Monitor::harcCount() {
	return send<Command::harccount>();
}



size_t Monitor::branchCount() {
	return send<Command::branchcount>();
}



float Monitor::followsPerSecond() {
	return send<Command::followps>();
}



float Monitor::activationsPerSecond() {
	return send<Command::activeps>();
}

float Monitor::processedPerSecond() {
	return send<Command::procps>();
}


