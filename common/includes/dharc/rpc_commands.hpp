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
	harccount,
	branchcount,
	followcount,
	followps,
	activeps,
	procps,
	makeharc,
	makeinputblock,
	activate,
	writeblock,
	end
};

/**
 * Corresponding types for each command (in the same order as commands are
 * specified in the Command enum struct).
 */
typedef tuple<
	bool(*)(),  // nop
	int(*)(),  // version
	size_t(*)(),  // harccount
	size_t(*)(),  // branchcount
	size_t(*)(),  // followcount
	float(*)(),  // followps
	float(*)(),  // activeps
	float(*)(),  // procps
	Node(*)(),  // makeharc
	Node(*)(const size_t &, const size_t &),  // makeinputblock
	bool(*)(const Node &, const float &),  // activate
	bool(*)(const Node &, const vector<float>&)  // writeblock
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_RPC_COMMANDS_HPP_

