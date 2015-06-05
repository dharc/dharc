/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_HPP_
#define DHARC_RPC_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <type_traits>

#include "dharc/rpc_commands.hpp"
#include "dharc/rpc_packer.hpp"

#include "zmq.hpp"

using dharc::rpc::Command;
using dharc::rpc::commands_t;
using dharc::rpc::Packer;

namespace dharc {

class Rpc {
	public:
	Rpc(const char *addr, int port);
	virtual ~Rpc();

	protected:
	/**
	 * Send an RPC command to the server. The arguments must match those expected
	 * for the given command, as must the return type. See the commands_t tuple
	 * for the correct types.
	 */
	template<Command C, typename... A>
	auto send(const A&... args) {
		using cmd_type =
		typename std::tuple_element<static_cast<int>(C), commands_t>::type;
		using ret_type =
		typename std::result_of<cmd_type(A...)>::type;

		// Make sure arguments are correct for this command
		static_assert(std::is_same<ret_type(*)(const A&...), cmd_type>::value,
			"Incorrect RPC Arguments");

		// Pack the command number and arguments
		std::ostringstream os(std::ios_base::out | std::ios_base::binary);
		packcmd(os, C);
		pack(os, args...);

		// Send and then unpack return value
		std::istringstream is(send(os.str()),
			std::ios_base::in | std::ios_base::binary);
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

		// Make sure no arguments are expected.
		static_assert(std::is_same<ret_type(*)(), cmd_type>::value,
			"Incorrect RPC Arguments");

		// Pack the command number and arguments
		std::ostringstream os(std::ios_base::out | std::ios_base::binary);
		packcmd(os, C);
		pack(os);

		// Send and then unpack return value
		std::istringstream is(send(os.str()),
			std::ios_base::in | std::ios_base::binary);
		return Packer<ret_type>::unpack(is);
	}


	private:
	/* Send a string request and get a string reply from the socket */
	std::string send(const std::string &s);

	/* One argument base case */
	template<typename F>
	void pack_(std::ostream &os, const F &first) {
		Packer<F>::pack(os, first);
	}

	/* Recursively pack the arguments into a (json) stream */
	template<typename F, typename... Args>
	void pack_(std::ostream &os, const F &first, const Args&... args) {
		pack_(os, first);
		os << ',';
		pack_(os, args...);
	}

	template<typename... Args>
	void pack(std::ostream &os, const Args&... args) {
		os << ", \"args\": [";
		pack_(os, args...);
		os << "]}";
	}

	inline void pack(std::ostream &os) {
		os << ", \"args\": []}";
	}

	inline void packcmd(std::ostream &os, Command c) {
		os << "{\"c\": " << static_cast<int>(c);
	}

	zmq::socket_t sock_;
	std::string uri_;
};
};  // namespace dharc

#endif  // DHARC_RPC_HPP_

