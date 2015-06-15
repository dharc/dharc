/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_SENSE_HPP_
#define DHARC_SENSE_HPP_

#include "dharc/rpc.hpp"

using std::pair;

namespace dharc {

class Sense : public dharc::Rpc {
	public:
	Sense(const char *addr, int port);
	~Sense();

	void makeInputBlock(size_t w, size_t h, Node &b);

	void writeInput(const Node &b,
					const vector<float> &values);

	vector<pair<float,Node>> readStrong(const Node &b, float active);
};

};

#endif  // DHARC_SENSE_HPP_

