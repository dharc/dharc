#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "fdsb/nid.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

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

void command_query(const vector<string> &e) {
	if (e.size() != 3) {
		std::cout << "  Query command expects 2 arguments." << std::endl;
	} else {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Nid r = fabric.get(t1,t2).query();
		std::cout << "  " << r << std::endl;
	}
}

void command_define(const vector<string> &e) {
	if (e.size() == 4) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Nid h = Nid::from_string(e[3]);
		fabric.get(t1,t2).define(h);
	} else {
		std::cout << "  Define command expects 3 arguments." << std::endl;
	}
}

void command_partners(const vector<string> &e) {
	if (e.size() != 2) {
		std::cout << "  Partners command expects 1 argument." << std::endl;
	} else {
		Nid n1 = Nid::from_string(e[1]);
		auto part = fabric.partners(n1);
		for (auto i : part) {
			std::cout << "  - " << *i << std::endl;
		}
	}
}

map<string, void (*)(const vector<string>&)> commands = {
		{ "query", command_query },
		{ "define", command_define },
		{ "partners", command_partners }
};

void interactive() {
	std::cout << "F-DSB Interactive Mode:" << std::endl;

	while (true) {
		string line;
		std::cout << "> ";
		std::cout.flush();
		std::getline(std::cin,line);

		vector<string> elems = split(line, ' ');
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
