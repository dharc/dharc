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

	Node makeHarc();
	void makeHarcs(int count, Node &first, Node &last);

	void activate(const Node &first,
					const Node &last,
					const vector<float> &values);
};

};

#endif  // DHARC_SENSE_HPP_

