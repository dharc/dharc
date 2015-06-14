/*
 * Copyright 2015 Nicolas Pope
 */

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

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
	zmq::message_t req(const_cast<char*>(s.data()), s.size(), 0, 0);

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
		int retry = 500;
		try {
			while (retry > 0 && !sock_.recv(&rep, ZMQ_NOBLOCK)) {
				--retry;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			if (retry == 0) {
				sock_.disconnect(uri_.c_str());
				cout << "Server unreachable!\n";
				exit(1);
			}

			break;
		} catch (zmq::error_t err) {
			cout << "ZMQ receive error: " << err.what() << "\n";
		}
	}

	return std::string((const char*)rep.data(), rep.size());
}

