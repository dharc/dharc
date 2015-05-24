/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMON_H_
#define DHARC_RPC_COMMON_H_

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <type_traits>

#include <cassert>

#include "dharc/rpc_commands.hpp"
#include "dharc/rpc_packer.hpp"

namespace dharc {
namespace rpc {

bool connect(const char *addr);

bool disconnect();

/* ==== INTERNALS =========================================================== */
namespace intern {
/* Send a string request and get a string reply from the socket */
std::string send(const std::string &s);

/* One argument base case */
template<typename F>
void pack(std::ostream &os, F first) {
	Packer<F>::pack(os, first);
}

/* Recursively pack the arguments into a (json) stream */
template<typename F, typename... Args>
void pack(std::ostream &os, F first, Args... args) {
	pack(os, first);
	os << ',';
	pack(os, args...);
}
};  // namespace intern
/* ========================================================================== */

/**
 * Send an RPC command to the server. The arguments must match those expected
 * for the given command, as must the return type. See the commands_t tuple
 * for the correct types.
 */
template<Command C, typename... Args>
auto send(const Args&... args) {
	using cmd_type =
	typename std::tuple_element<static_cast<int>(C), commands_t>::type;

	using ret_type =
	typename std::result_of<cmd_type(Args...)>::type;

	// Make sure arguments are correct for this command
	static_assert(std::is_same<ret_type(*)(const Args&...), cmd_type>::value,
		"Incorrect RPC Arguments");

	// Pack the command number and arguments
	std::stringstream os;
	os << "{\"c\": " << static_cast<int>(C);
	os << ", \"args\": [";
	intern::pack(os, args...);
	os << "]}";

	// Send and then unpack return value
	std::stringstream is(intern::send(os.str()));
	return Packer<ret_type>::unpack(is);
}

/**
 * A version of send for commands that take no arguments.
 */
template<Command C>
auto send() {
	using cmd_type =
	typename std::tuple_element<static_cast<int>(C), commands_t>::type;
	using ret_type =
	typename std::result_of<cmd_type()>::type;

	// Pack the command number and arguments
	std::stringstream os;
	os << "{\"c\": " << static_cast<int>(C);
	os << ", \"args\": []}";

	// Send and then unpack return value
	std::stringstream is(intern::send(os.str()));
	return Packer<ret_type>::unpack(is);
}

};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMON_H_ */

