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
using std::pair;

namespace dharc {
namespace rpc {
/**
 * Available RPC command constants, telling the server which procedure to
 * call to deal with the clients request.
 */
enum struct Command : int {
	nop,
	version,
	write2dsigned,
	reform2dsigned,
	end
};

/**
 * Corresponding types for each command (in the same order as commands are
 * specified in the Command enum struct).
 */
typedef tuple<
	bool(*)(),  // nop
	int(*)(),  // version
	bool(*)(const size_t &, const vector<int8_t> &, const size_t &, const size_t &),
	vector<int8_t>(*)(const size_t &, const size_t &, const size_t &)
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_RPC_COMMANDS_HPP_

