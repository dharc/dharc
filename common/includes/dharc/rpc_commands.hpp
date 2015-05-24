/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMANDS_H_
#define DHARC_RPC_COMMANDS_H_

#include <tuple>
#include <vector>
#include <list>

#include "dharc/nid.hpp"

using std::vector;
using std::list;
using std::tuple;
using dharc::Nid;

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
	Nid(*)(const Nid&, const Nid&),
	bool(*)(const Nid&, const Nid&, const Nid&),
	bool(*)(const Nid&, const Nid&, const vector<vector<Nid>>&),
	list<Nid>(*)(const Nid&)
> commands_t;

};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMANDS_H_ */

