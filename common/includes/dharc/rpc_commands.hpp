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
	// write_many_one,
	// write_many_many,
	// write_range_one,
	// write_range_many
	write_rangerange_many,
	unique_block,
	log,
	end
};

/**
 * Corresponding types for each command (in the same order as commands are
 * specified in the Command enum struct).
 */
typedef tuple<
	bool(*)(),                                  // nop
	int(*)(),                                   // version
	Node(*)(const Tail&),                       // query
	bool(*)(const Tail&, const Node&),          // define_const
	bool(*)(const Tail&, const vector<Node>&),  // define
	vector<Tail>(*)(const Node&, const int&),   // partners
	Node(*)(),                                  // unique
	size_t(*)(),                                // linkcount
	size_t(*)(),                                // nodecount
	float(*)(),                                 // changes
	float(*)(),                                 // queries
	bool(*)(const vector<Node>&, const Node&,   // write_rangerange_many
			const Node&,
			const vector<vector<Node>>&),
	vector<Node>(*)(const int &),               // unique_block
	vector<Tail>(*)(const int &)                // change log
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_RPC_COMMANDS_HPP_

