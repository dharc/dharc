/*
 * Copyright 2015 Nicolas Pope
 */

#include <string>
#include <iostream>

#include "zmq.hpp"
#include "dharc/rpc.hpp"

using dharc::Rpc;
using std::cout;

namespace {
zmq::context_t context(1);
};

Rpc::Rpc(const char *addr, int port) : sock_(context, ZMQ_REQ) {
	uri_ = "tcp://";
	uri_ += addr;
	uri_ += ':';
	uri_ += std::to_string(port);

	sock_.connect(uri_.c_str());

	// Do a version check!
	if (send<Command::version>() != static_cast<int>(Command::end)) {
		cout << "!!! dharcd uses different version of rpc protocol !!!";
		cout << std::endl;
	}
}

Rpc::~Rpc() {
}

std::string Rpc::send(const std::string &s) {
	zmq::message_t req(s.size()+1);
	memcpy(req.data(), s.data(), s.size()+1);

	sock_.send(req);

	zmq::message_t rep;
	sock_.recv(&rep);
	return std::string((const char*)rep.data());
}

