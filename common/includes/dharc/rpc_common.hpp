/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMON_H_
#define DHARC_RPC_COMMON_H_

#include <string>
#include <sstream>
#include <vector>

#include "dharc/rpc_commands.hpp"
#include "dharc/rpc_packer.hpp"

namespace dharc {
namespace rpc {

bool connect(const char *addr);

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

/**
 * Send an RPC command to the server, that takes no arguments.
 */
template<typename R>
R send(Command c) {
	std::stringstream os;
	os << "{\"c\": " << static_cast<int>(c) << '}';
	std::stringstream is(send(os.str()));
	return Packer<R>::unpack(is);
}

/**
 * Send an RPC command to the server, that takes one or more arguments.
 */
template<typename R, typename... Args>
R send(Command c, Args... args) {
	std::stringstream os;
	os << "{\"c\": " << static_cast<int>(c);
	os << ", \"args\": [";
	pack(os, args...);
	os << "]}";
	std::stringstream is(send(os.str()));
	return Packer<R>::unpack(is);
}

bool disconnect();

};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMON_H_ */

