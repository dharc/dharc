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

	void write(const Node &n1, const Node &n2, const Node &h);
	void write(const Node &n, const Node &first, const Node *data, int count);
};

};

#endif  // DHARC_SENSE_HPP_

