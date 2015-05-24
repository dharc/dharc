/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_ARCH_HPP_
#define DHARC_ARCH_HPP_

#include <vector>
#include <list>

#include "dharc/nid.hpp"
#include "dharc/rpc_common.hpp"

using std::vector;
using std::list;

namespace dharc {
	void start(int argc, char *argv[]);
	void stop();

	Nid query(const Nid &a, const Nid &b);
	void define(const Nid &a, const Nid &b, const Nid &h);
	void define(const Nid &a, const Nid &b, const vector<vector<Nid>> &p);
	list<Nid> partners(const Nid &a);
};

#endif  /* DHARC_ARCH_HPP_ */

