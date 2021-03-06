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
using std::pair;


namespace {
struct {
	int interactive;
	int no_info;
	int no_labels;
	int stats;
	int changes;
	vector<Node> params;
} config {
	0,
	0,
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
	{"set", 1, nullptr, 1001},
	{"stats", 1, nullptr, 1002},
	{"log", 1, nullptr, 1003},
	{"interactive", 0, &config.interactive, 1},
	{"port", 1, nullptr, 'p'},
	{"host", 1, nullptr, 'h'},
	{"help", 0, nullptr, 1000},
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

	vector<pair<int, string>> delayed;

	signal(SIGINT, signal_handler);

	while ((o = getopt_long(argc, argv, "f:ic:h:p:", opts, nullptr)) != -1) {
		switch(o) {
		case 'i': config.interactive = 1; break;
		case 'h': host = optarg; break;
		case 'p': port = std::stoi(optarg); break;
		case 1000: std::cout << helpmsg; break;
		case 1001: delayed.push_back({0, string(optarg)}); break;
		case 1002: if (string(optarg) == "all") config.stats = 0xFFFF; break;
		case 1003: config.changes = stoi(string(optarg)); break;
		case ':': cout << "Option '" << optopt << "' requires an argument\n";
					break;
		default: break;
		}
	}

	dharc::Monitor monitor(host, port);


	if (config.stats == 0xFFFF) {
		cout << "Harc Count: " << monitor.harcCount() << std::endl;
		cout << "Branch Count: " << monitor.branchCount() << std::endl;
		cout << "Follows (s): " << (monitor.followsPerSecond() / 1000000.0f);
		cout << "M/s" << std::endl;
		cout << "Activations (s): " << (monitor.activationsPerSecond() / 1000000.0f);
		cout << "M/s" << std::endl;
		cout << "Processed (s): " << (monitor.processedPerSecond() / 1000000.0f);
		cout << "M/s" << std::endl;
	}


	if (config.interactive) {
		// interactive();
	}


	return 0;
}

