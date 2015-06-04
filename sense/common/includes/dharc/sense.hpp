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

	Node unique();
	void unique(int count, Node &first, Node &last);

	void write(const vector<Node> &common,
				const Node &r1,
				const Node &r2,
				const vector<vector<Node>> &values);
};

};

#endif  // DHARC_SENSE_HPP_

