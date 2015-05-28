/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_ARCH_HPP_
#define DHARC_ARCH_HPP_

#include <vector>
#include <list>

#include "dharc/node.hpp"
#include "dharc/rpc_common.hpp"

using std::vector;
using std::list;

namespace dharc {
	void start(int argc, char *argv[]);
	void stop();

	Node unique();
	Node query(const Node &a, const Node &b);
	void define(const Node &a, const Node &b, const Node &h);
	void define(const Node &a, const Node &b, const vector<vector<Node>> &p);
	list<Node> partners(const Node &a);
};

#endif  // DHARC_ARCH_HPP_

