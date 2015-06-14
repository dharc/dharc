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

void Sense::makeInputBlock(size_t w, size_t h, Node &b) {
	b = send<Command::makeinputblock>(w, h);
}

void Sense::writeInput(const Node &b,
						const vector<float> &values) {
	send<Command::writeblock>(b, values);
}

vector<Node> Sense::readStrong(const Node &b, float active) {
	return send<Command::readstrong>(b, active);
}

