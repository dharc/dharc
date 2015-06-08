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

Node Sense::makeHarc() {
	return send<Command::makeharc>();
}

void Sense::makeHarcs(int count, Node &first, Node &last) {
	vector<Node> res = send<Command::makeharcs>(count);
	first = res[0];
	last = res[1];
}

void Sense::activate(const Node &first,
						const Node &last,
						const vector<float> &values) {
	send<Command::activateblock>(first, last, values);
}
