/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMON_H_
#define DHARC_RPC_COMMON_H_

#include "zmq.hpp"

#include <string>
#include <sstream>

namespace dharc {
namespace rpc {
	enum struct Command : int {
		query,
		define_const,
		define,
		partners,
		details
	};

	extern zmq::socket_t rpc_sock;

	bool connect(const char *addr);

	template<typename F>
	void pack(std::ostream &os, F first) {
		os << '"' << first << '"';
	}

	template<typename F, typename... Args>
	void pack(std::ostream &os, F first, Args... args) {
		os << '"' << first << "\",";
		pack(os, args...);
	}

	template<typename R, typename... Args>
	R send(Command c, Args... args) {
		std::stringstream os;
		os << "{\"c\": " << static_cast<int>(c);
		os << ", \"args\": [";
		pack(os,args...);
		os << "]}";
		std::string s = os.str();
		zmq::message_t req(s.size()+1);
		memcpy(req.data(), s.data(), s.size()+1);
		rpc_sock.send(req);

		zmq::message_t rep;
		rpc_sock.recv(&rep);
		return R::from_string((const char*)rep.data());
	}

	bool disconnect();
};
};

#endif  /* DHARC_RPC_COMMON_H_ */

