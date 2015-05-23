/*
 * Copyright 2015 Nicolas Pope
 */

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "dharc/nid.hpp"
#include "dharc/arch.hpp"

using dharc::Nid;

using std::map;
using std::string;
using std::vector;
using std::stringstream;
using std::cout;

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

/*vector<vector<Nid>> build_path(const vector<string> &e, int ix) {
	vector<vector<Nid>> p;
	vector<Nid> cursub;

	for (auto i = ix; i < static_cast<int>(e.size()); ++i) {
		if (e[i] == "}") {
			break;
		} else if (e[i] == "(") {
			continue;
		} else if (e[i] == ")") {
			p.push_back(cursub);
			cursub.clear();
		} else {
			cursub.push_back(Nid::from_string(e[i]));
		}
	}

	if (cursub.size() > 0) {
		p.push_back(cursub);
	}

	return p;
}*/

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
		//Nid r = fdsb::fabric.get(t1, t2).query();
		Nid r = dharc::query(t1, t2);
		std::cout << "  " << r << std::endl;
	}
}

/*void command_define(const vector<string> &e) {
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
		cout << "  Define command expects 3 or more arguments." << std::endl;
	}
}

void command_partners(const vector<string> &e) {
	if (e.size() < 2) {
		cout << "  Partners command expects 1 or more arguments." << std::endl;
	} else {
		if (e[1] == "all") {
			Nid n1 = Nid::from_string(e[2]);
			auto part = fdsb::fabric.partners(n1);
			for (auto i : part) {
				cout << "  - " << *i << std::endl;
			}
		} else {
			vector<Nid> nodes;
			for (int i = 1; i < static_cast<int>(e.size()); ++i) {
				Nid n1 = Nid::from_string(e[i]);
				nodes.push_back(n1);
			}
			auto part = Framer::select_partners(nodes, 10);
			for (auto i : part) {
				cout << "  - [" << i->tail().first << " "
						<< i->tail().second << "]" << std::endl;
			}
		}
	}
}

void command_path(const vector<string> &e) {
	if (e.size() >= 3) {
		cout << "  " << fdsb::fabric.path(build_path(e, 1)) << std::endl;
	} else {
		cout << "  Path command expects 2 or more arguments." << std::endl;
	}
}

void command_array(const vector<string> &e) {
	if (e.size() >= 3) {
		Nid r = Nid::unique();
		for (auto i = 1; i < static_cast<int>(e.size()); ++i) {
			r[Nid::from_int(i-1)] = Nid::from_string(e[i]);
		}
		cout << "  " << r << std::endl;
	} else {
		cout << "  Array command expects 2 or more arguments." << std::endl;
	}
}

void command_dependants(const vector<string> &e) {
	if (e.size() != 3) {
		cout << "  Dependants command expects 2 arguments." << std::endl;
	} else {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		auto deps = fdsb::fabric.get(t1, t2).dependants();
		for (auto i : deps) {
			cout << "  - " << *i << std::endl;
		}
	}
}

void command_details(const vector<string> &e) {
	if (e.size() == 3) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Harc &h = fdsb::fabric.get(t1, t2);

		cout << "    tail: " << t1 << ", " << t2 << std::endl;
		cout << "    significance: " << h.significance() << std::endl;
		if (h.check_flag(Harc::Flag::defined)) {
			cout << "    definition: ";
			cout << h.definition()->to_string() << std::endl;
		} else {
			cout << "    value: " << h.query() << std::endl;
		}
		cout << "    last query: " << h.last_query() << "s" << std::endl;
	} else {
		cout << "  Details command expects 2 arguments." << std::endl;
	}
}*/

/* ========================================================================== */

/*
 * Map command words to associated command function for processing.
 */
map<string, void (*)(const vector<string>&)> commands = {
		{ "%query", command_query },
		// { "%define", command_define },
		// { "%partners", command_partners },
		// { "%path", command_path },
		// { "%array", command_array },
		// { "%dependants", command_dependants },
		// { "%details", command_details }
		// %string
		// %sigpath
		// %dependants
		// %dependencies
		// %definition
};

Nid parse_dsbscript(const vector<string> &tokens) {
	Nid cur = Nid::from_string(tokens[0]);
	Nid other;
	Nid old;

	for (int i = 1; i < static_cast<int>(tokens.size()); ++i) {
		if (tokens[i] == "=") {
			if (tokens[i+1] == "{") {
				/*++i;
				fdsb::fabric.get(old, other).define(
					build_path(tokens, ++i));
				++i;
				cur = old;*/
			} else {
				dharc::define(old, other, Nid::from_string(tokens[++i]));
				cur = old;
			}
		} else {
			old = cur;
			other = Nid::from_string(tokens[i]);
			cur = dharc::query(old, other);
		}
	}

	return cur;
}

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

		if (line.size() == 0) continue;

		vector<string> elems = split(line, ' ');

		if (elems[0] == "%exit") return;

		if (elems[0][0] == '%') {
			auto it = commands.find(elems[0]);
			if (it != commands.end()) {
				it->second(elems);
			} else {
				std::cout << "  Unrecognised command: " << elems[0] << std::endl;
			}
		} else {
			cout << "  " << parse_dsbscript(elems) << std::endl;
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
			default:
				cout << "Unrecognised command line argument." << std::endl;
				return -1;
			}
		}
		++i;
	}

	dharc::start(argc,argv);

	if (is_i) {
		interactive();
	}

	dharc::stop();

	return 0;
}

