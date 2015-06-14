/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_SENSE_HPP_
#define DHARC_SENSE_HPP_

#include "dharc/rpc.hpp"

namespace dharc {

class Sense : public dharc::Rpc {
	public:
	Sense(const char *addr, int port);
	~Sense();

	void makeInputBlock(size_t w, size_t h, Node &b);

	void writeInput(const Node &b,
					const vector<float> &values);
};

};

#endif  // DHARC_SENSE_HPP_

