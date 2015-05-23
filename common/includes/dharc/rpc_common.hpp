/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMON_H_
#define DHARC_RPC_COMMON_H_

namespace dharc {
namespace rpc {
	enum struct Command {
		query,
		define_const,
		define,
		partners,
		details
	};

	bool connect(const char *addr);

	template<typename R, typename... Args>
	R send(Command c, Args... args);

	Nid query(const Nid &a, const Nid &b) {
		return send(Command::query, a, b);
	}

	bool disconnect();
};
};

#endif  /* DHARC_RPC_COMMON_H_ */

