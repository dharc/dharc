/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMON_H_
#define DHARC_RPC_COMMON_H_

#include <string>
#include <sstream>
#include <vector>

#include "zmq.hpp"
#include "dharc/rpc_commands.hpp"
#include "dharc/rpc_packer.hpp"

namespace dharc {
namespace rpc {


	extern zmq::socket_t rpc_sock;

	bool connect(const char *addr);

	template<typename F>
	void pack(std::ostream &os, F first) {
		Packer<F>::pack(os, first);
	}

	template<typename F, typename... Args>
	void pack(std::ostream &os, F first, Args... args) {
		pack(os, first);
		os << ',';
		pack(os, args...);
	}

	template<typename R, typename... Args>
	R send(Command c, Args... args) {
		std::stringstream os;
		os << "{\"c\": " << static_cast<int>(c);
		os << ", \"args\": [";
		pack(os, args...);
		os << "]}";

		std::string s = os.str();
		zmq::message_t req(s.size()+1);
		memcpy(req.data(), s.data(), s.size()+1);
		rpc_sock.send(req);

		zmq::message_t rep;
		rpc_sock.recv(&rep);
		std::stringstream is(std::string((const char*)rep.data()));
		return Packer<R>::unpack(is);
	}

	bool disconnect();
};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMON_H_ */

