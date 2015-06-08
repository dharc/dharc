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

	size_t harcCount();
	size_t branchCount();
	float followsPerSecond();
	float activationsPerSecond();

	/* Statistics functions */
	/* Stream functions */
};
};  // namespace dharc

#endif  // DHARC_MONITOR_HPP_

