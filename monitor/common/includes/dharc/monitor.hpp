/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_MONITOR_HPP_
#define DHARC_MONITOR_HPP_

#include <vector>
#include <list>
#include <string>

#include "dharc/node.hpp"
#include "dharc/rpc.hpp"
#include "dharc/tail.hpp"

using std::vector;
using std::list;

namespace dharc {
class Monitor : public dharc::Rpc {
	public:
	Monitor(const char *host, int port);
	~Monitor();

	Node unique();
	Node query(const dharc::Tail &tail);
	void define(const dharc::Tail &tail, const Node &h);
	void define(const dharc::Tail &tail, const vector<Node> &p);
	vector<dharc::Tail> partners(const Node &a, int count);

	size_t linkCount();
	size_t nodeCount();
	float queriesPerSecond();
	float changesPerSecond();

	/* Statistics functions */
	/* Stream functions */
};
};  // namespace dharc

#endif  // DHARC_MONITOR_HPP_

