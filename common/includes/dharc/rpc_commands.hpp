/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMANDS_HPP_
#define DHARC_RPC_COMMANDS_HPP_

#include <tuple>
#include <vector>
#include <list>

#include "dharc/node.hpp"
#include "dharc/tail.hpp"

using std::vector;
using std::list;
using std::tuple;
using dharc::Node;
using dharc::Tail;

namespace dharc {
namespace rpc {
/**
 * Available RPC command constants, telling the server which procedure to
 * call to deal with the clients request.
 */
enum struct Command : int {
	nop,
	version,
	query,
	define_const,
	define,
	partners,
	unique,
	linkcount,
	nodecount,
	changes,
	queries,
	// write_one_one,  // same as define_const
	// write_many_one,
	// write_many_many,
	// write_range_one,
	// write_range_many
	end
};

/**
 * Corresponding types for each command (in the same order as commands are
 * specified in the Command enum struct).
 */
typedef tuple<
	bool(*)(),
	int(*)(),
	Node(*)(const Node&, const Node&),
	bool(*)(const Node&, const Node&, const Node&),
	bool(*)(const Node&, const Node&, const vector<vector<Node>>&),
	vector<Tail>(*)(const Node&, const int&),
	Node(*)(),
	size_t(*)(),
	size_t(*)(),
	float(*)(),
	float(*)()
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_RPC_COMMANDS_HPP_

