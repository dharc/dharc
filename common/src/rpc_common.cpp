/*
 * Copyright 2015 Nicolas Pope
 */

#include <string>

#include "zmq.hpp"
#include "dharc/rpc_common.hpp"

static zmq::context_t context(1);
zmq::socket_t rpc_sock(context, ZMQ_REQ);

bool dharc::rpc::connect(const char *addr) {
	rpc_sock.connect(addr);
	return true;
}

std::string dharc::rpc::send(const std::string &s) {
	zmq::message_t req(s.size()+1);
	memcpy(req.data(), s.data(), s.size()+1);
	rpc_sock.send(req);

	zmq::message_t rep;
	rpc_sock.recv(&rep);
	return std::string((const char*)rep.data());
}

