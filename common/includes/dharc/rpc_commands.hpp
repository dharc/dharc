/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMANDS_HPP_
#define DHARC_RPC_COMMANDS_HPP_

#include <tuple>
#include <vector>
#include <list>

#include "dharc/node.hpp"

using std::vector;
using std::list;
using std::tuple;
using dharc::Node;

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
	// details,
	// path,
	// paths,
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
	list<Node>(*)(const Node&),
	Node(*)()
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_RPC_COMMANDS_HPP_

