/*
 * Copyright 2015 Nicolas Pope
 */

#include <getopt.h>
#include <csignal>

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "dharc/node.hpp"
#include "dharc/monitor.hpp"
#include "dharc/labels.hpp"


using dharc::Node;

using std::map;
using std::string;
using std::vector;
using std::stringstream;
using std::cout;


namespace {
struct {
	int interactive;
	int no_info;
	int no_labels;
	vector<Node> params;
} config {
	0,
	0,
	0,
	{}
};



void signal_handler(int param) {
	std::cout << std::endl;

	exit(0);
}



option opts[] = {
	{"interactive", 0, &config.interactive, 1},
	{"noinfo", 0, &config.no_info, 1},
	{"cmd", 1, nullptr, 'c'},
	{"port", 1, nullptr, 'p'},
	{"host", 1, nullptr, 'h'},
	{"help", 0, nullptr, 1000},
	{"file", 1, nullptr, 'f'},
	{"param", 1, nullptr, '*'},
	{"nolabels", 0, &config.no_labels, 1},
	{nullptr, 0, nullptr, 0}
};



const char *helpmsg =
"usage: dharc-arch [-i | --interactive] [--noinfo] [--param=<node>]\n"
"                  [-h <host> | --host=<host>] [-p <port> | --port=<port>]\n"
"                  [-f <file> | --file=<file>] [--nolabels] [--help]\n"
"\n"
;

};  // namespace



int main(int argc, char *argv[]) {
	int o;
	std::stringstream ss;
	std::ifstream ifs;
	const char *host = "localhost";
	int port = 7878;

	signal(SIGINT, signal_handler);

	while ((o = getopt_long(argc, argv, "f:ic:h:p:", opts, nullptr)) != -1) {
		switch(o) {
		case 'i': config.interactive = 1; break;
		case 'h': host = optarg; break;
		case 'p': port = std::stoi(optarg); break;
		case 1000: std::cout << helpmsg; break;
		case '*': config.params.push_back(Node(optarg)); break;
		case ':': cout << "Option '" << optopt << "' requires an argument\n";
					break;
		default: break;
		}
	}

	dharc::Monitor monitor(host, port);

	if (config.interactive) {
		// interactive();
	}


	return 0;
}

