/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_COMMANDS_H_
#define DHARC_RPC_COMMANDS_H_

namespace dharc {
namespace rpc {
	enum struct Command : int {
		query,
		define_const,
		define,
		partners,
		details
	};
};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_COMMANDS_H_ */

