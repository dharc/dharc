/*
 * Copyright 2015 Nicolas Pope
 */

#include <iostream>
#include <sstream>
#include <string>
#include <csignal>

#include "zmq.hpp"
#include "dharc/rpc_server.hpp"
#include "dharc/fabric.hpp"

using std::cout;
using std::string;
using dharc::Fabric;

void signal_handler(int param) {
	std::cout << std::endl;

	exit(0);
}


int main(int argc, char *argv[]) {
	int i = 1;
	zmq::message_t msg;

	signal(SIGINT, signal_handler);

	Fabric::initialise();

	// Process command line arguments.
	while (i < argc) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			default:
				cout << "Unrecognised command line argument." << std::endl;
				return -1;
			}
		}
		++i;
	}

	zmq::context_t context(1);

	zmq::socket_t rpc(context, ZMQ_REP);
	rpc.bind("tcp://*:7878");

	zmq::pollitem_t items [] = {
		{ rpc, 0, ZMQ_POLLIN, 0 }
	};

	while (true) {
		try {
			zmq::poll(&items[0], 1, -1);
		} catch (zmq::error_t ex) {
			cout << "zmq error\n";
			continue;
		}

		if (items[0].revents & ZMQ_POLLIN) {
			while (true) {
				try {
					rpc.recv(&msg);
					break;
				} catch (zmq::error_t err) {}
			}


			std::istringstream is(string((const char*)msg.data(), msg.size()),
									std::ios_base::in | std::ios_base::binary);
			std::ostringstream os(std::ios_base::out | std::ios_base::binary);

			dharc::rpc::process_msg(is, os);

			string res = os.str();
			zmq::message_t rep(const_cast<char*>(res.data()), os.tellp(), 0);

			while (true) {
				try {
					rpc.send(res.data(), os.tellp());
					break;
				} catch (zmq::error_t err) {
					std::cout << "Oops, send error\n";
				}
			}

			items[0].revents = 0;
		}
	}

	return 0;
}
