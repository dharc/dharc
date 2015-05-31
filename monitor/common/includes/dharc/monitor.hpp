/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_MONITOR_HPP_
#define DHARC_MONITOR_HPP_

#include <vector>
#include <list>
#include <string>

#include "dharc/node.hpp"
#include "dharc/rpc_common.hpp"

using std::vector;
using std::list;

namespace dharc {
class Monitor {
	public:
	Monitor(const char *host, int port);
	~Monitor();

	Node unique();
	Node query(const Node &a, const Node &b);
	void define(const Node &a, const Node &b, const Node &h);
	void define(const Node &a, const Node &b, const vector<vector<Node>> &p);
	list<Node> partners(const Node &a);

	size_t linkCount();
	size_t nodeCount();
	float queriesPerSecond();
	float changesPerSecond();

	/* Statistics functions */
	/* Stream functions */
	private:
	std::string uri_;
};
};  // namespace dharc

#endif  // DHARC_MONITOR_HPP_

