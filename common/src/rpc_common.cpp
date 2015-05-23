/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/rpc_common.hpp"

static zmq::context_t context(1);
zmq::socket_t dharc::rpc::rpc_sock(context, ZMQ_REQ);

bool dharc::rpc::connect(const char *addr) {
	rpc_sock.connect(addr);
	return true;
}

