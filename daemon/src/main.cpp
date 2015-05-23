/*
 * Copyright 2015 Nicolas Pope
 */

#include <iostream>

#include "zmq.hpp"

using std::cout;

int main(int argc, char *argv[]) {
	int i = 1;

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
		zmq::message_t msg;
		zmq::poll(&items[0], 1, -1);

		if (items[0].revents & ZMQ_POLLIN) {
			rpc.recv(&msg);
			cout << "Message: " << (const char*)msg.data() << std::endl;

			std::string res = "[666]";
			zmq::message_t rep(res.size()+1);
			memcpy(rep.data(), res.data(), res.size()+1);
			rpc.send(rep);
		}
	}

	return 0;
}
