/*
 * Copyright 2015 Nicolas Pope
 */

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "fdsb/nid.hpp"
#include "fdsb/fabric.hpp"

using fdsb::Nid;

using std::map;
using std::string;
using std::vector;
using std::stringstream;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

fdsb::Path build_path(const vector<string> &e, int ix) {
	fdsb::Path p;

	for (auto i = ix; i < static_cast<int>(e.size()); ++i) {
		p.push_back({Nid::from_string(e[i])});
	}

	return p;
}

/* ======== Command Functions =============================================== */

/*
 * Query a hyperarc for its head node. Two node id's are given as arguments
 * which identify the tail of the hyperarc.
 */
void command_query(const vector<string> &e) {
	if (e.size() != 3) {
		std::cout << "  Query command expects 2 arguments." << std::endl;
	} else {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Nid r = fdsb::fabric.get(t1, t2).query();
		std::cout << "  " << r << std::endl;
	}
}

void command_define(const vector<string> &e) {
	// Just a constant definition
	if (e.size() == 4) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Nid h = Nid::from_string(e[3]);
		fdsb::fabric.get(t1, t2).define(h);

	// Path based definition
	} else if (e.size() > 4) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		fdsb::fabric.get(t1, t2).define(build_path(e, 3));

	} else {
		std::cout << "  Define command expects 3 or more arguments." << std::endl;
	}
}

void command_partners(const vector<string> &e) {
	if (e.size() != 2) {
		std::cout << "  Partners command expects 1 argument." << std::endl;
	} else {
		Nid n1 = Nid::from_string(e[1]);
		auto part = fdsb::fabric.partners(n1);
		for (auto i : part) {
			std::cout << "  - " << *i << std::endl;
		}
	}
}

void command_path(const vector<string> &e) {
	if (e.size() >= 3) {
		std::cout << "  " << fdsb::fabric.path(build_path(e, 1)) << std::endl;
	} else {
		std::cout << "  Path command expects 3 or more arguments." << std::endl;
	}
}

/* ========================================================================== */

/*
 * Map command words to associated command function for processing.
 */
map<string, void (*)(const vector<string>&)> commands = {
		{ "query", command_query },
		{ "define", command_define },
		{ "partners", command_partners },
		{ "path", command_path }
};

/*
 * Read command line entries when in interactive mode and pass to relevant
 * command function for processing.
 */
void interactive() {
	std::cout << "F-DSB Interactive Mode:" << std::endl;

	while (true) {
		string line;
		std::cout << "> ";
		std::cout.flush();
		std::getline(std::cin, line);

		vector<string> elems = split(line, ' ');

		if (elems[0] == "exit") return;

		auto it = commands.find(elems[0]);
		if (it != commands.end()) {
			it->second(elems);
		} else {
			std::cout << "  Unrecognised command: " << elems[0] << std::endl;
		}
	}
}

int main(int argc, char *argv[]) {
	int i = 1;
	bool is_i = false;

	while (i < argc) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'i': is_i = true; break;
			}
		}
		++i;
	}

	if (is_i) {
		interactive();
	}

	return 0;
}
