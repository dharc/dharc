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
	zmq::message_t req(const_cast<char*>(s.c_str()), s.size()+1, 0, 0);

	while (true) {
		try {
			sock_.send(req);
			break;
		} catch (zmq::error_t err) {
			cout << "ZMQ send error: " << err.what() << "\n";
		}
	}

	zmq::message_t rep;

	while (true) {
		try {
			sock_.recv(&rep);
			break;
		} catch (zmq::error_t err) {
			cout << "ZMQ receive error: " << err.what() << "\n";
		}
	}

	return std::string((const char*)rep.data());
}

