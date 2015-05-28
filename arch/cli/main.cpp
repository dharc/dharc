/*
 * Copyright 2015 Nicolas Pope
 */

#include <getopt.h>

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include "dharc/node.hpp"
#include "dharc/arch.hpp"
#include "dharc/script.hpp"

using dharc::Node;
using dharc::arch::Script;

using std::map;
using std::string;
using std::vector;
using std::stringstream;
using std::cout;


static struct {
	int interactive;
	int no_info;
	vector<Node> params;
} config {
	0,
	0,
	{}
};

static void execute(std::istream &ss, const char *source) {
	Script script(ss, source);

	if (config.no_info) {
		script.showInformation(false);
	} else {
		script.showInformation(true);
	}

	Node res = script(config.params);
	if (config.interactive) cout << "    ";
	cout << res << std::endl;
}

/*
 * Read command line entries when in interactive mode and pass to relevant
 * command function for processing.
 */
static void interactive() {
	std::cout << "Dharc Arch Interactive:" << std::endl;

	while (true) {
		string line;
		std::cout << "> ";
		std::cout.flush();
		std::getline(std::cin, line);
		line += "\n";
		if (line.size() == 0) continue;
		std::stringstream ss(line);
		execute(ss, "stdin");
	}
}

static option opts[] = {
	{"interactive", 0, &config.interactive, 1},
	{"noinfo", 0, &config.no_info, 1},
	{"cmd", 1, nullptr, 'c'},
	{"port", 1, nullptr, 'p'},
	{"host", 1, nullptr, 'h'},
	{"file", 1, nullptr, 'f'},
	{"param", 1, nullptr, '*'},
	{nullptr, 0, nullptr, 0}
};

int main(int argc, char *argv[]) {
	int o;
	std::stringstream ss;
	std::ifstream ifs;

	dharc::start(argc, argv);

	while ((o = getopt_long(argc, argv, "f:ic:h:p:", opts, nullptr)) != -1) {
		switch(o) {
		case 'c': ss.str(string(optarg)); execute(ss, "cli"); break;
		case 'i': config.interactive = 1; break;
		case 'h': break;
		case 'p': break;
		case '*': config.params.push_back(Node(optarg)); break;
		case 'f': ifs.open(optarg); execute(ifs, optarg); ifs.close(); break;
		case ':': cout << "Option '" << optopt << "' requires an argument\n"; break;
		default: break;
		}
	}

	if (config.interactive) {
		interactive();
	}

	dharc::stop();

	return 0;
}

