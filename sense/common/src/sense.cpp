/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/sense.hpp"

#include <vector>

using std::vector;
using dharc::Sense;
using std::pair;

Sense::Sense(const char *addr, int port) : Rpc(addr, port) {}

Sense::~Sense() {}

void Sense::write2D(
		RegionID regid,
		const vector<uint8_t> &values,
		size_t uw, size_t uh) {
	send<Command::write2d>(static_cast<size_t>(regid), values, uw, uh);
}

vector<uint8_t> Sense::reform2D(RegionID regid, size_t uw, size_t uh) {
	return send<Command::reform2d>(static_cast<size_t>(regid), uw, uh);
}

