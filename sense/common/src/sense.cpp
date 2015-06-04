/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/sense.hpp"

#include <vector>

using std::vector;
using dharc::Node;
using dharc::Sense;

Sense::Sense(const char *addr, int port) : Rpc(addr, port) {}

Sense::~Sense() {}

Node Sense::unique() {
	return send<Command::unique>();
}

void Sense::unique(int count, Node &first, Node &last) {
	vector<Node> res = send<Command::unique_block>(count);
	first = res[0];
	last = res[1];
}

void Sense::write(const vector<Node> &common,
				const Node &r1,
				const Node &r2,
				const vector<vector<Node>> &values) {
	send<Command::write_rangerange_many>(common, r1, r2, values);
}

