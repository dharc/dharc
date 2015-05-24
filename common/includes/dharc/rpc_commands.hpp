/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMANDS_H_
#define DHARC_RPC_COMMANDS_H_

namespace dharc {
namespace rpc {
/**
 * Available RPC command constants, telling the server which procedure to
 * call to deal with the clients request.
 */
enum struct Command : int {
	version,        // int :
	query,          // Nid : Nid, Nid
	define_const,   // bool : Nid, Nid, Nid
	define,         // bool : Nid, Nid, vector<vector<Nid>>
	partners,       // list<Nid> : Nid
	details,        // HarcStats : Nid, Nid
	path,           // Nid : vector<vector<Nid>>
	paths           // vector<Nid> : vector<vector<Nid>>
};
};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMANDS_H_ */

