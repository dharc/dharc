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
#include "dharc/nid.hpp"
#include "dharc/arch.hpp"
#include "dharc/parse.hpp"

using dharc::Nid;
using dharc::Parser;
using dharc::value_;
using dharc::word_;

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

vector<vector<Nid>> build_path(const vector<string> &e, int ix) {
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
}

/* ======== Command Functions =============================================== */

/*
 * Query a hyperarc for its head node. Two node id's are given as arguments
 * which identify the tail of the hyperarc.
 */
bool command_query(Parser &parse) {
	Nid t1;
	Nid t2;

	if (!parse(value_<Nid>{t1}, value_<Nid>{t2})) {
		parse.syntax_error("'%query' requires two node ids");
		return true;
	}
	if (!parse(';')) {
		parse.warning("Expected ';'");
	}
	Nid r = dharc::query(t1, t2);
	std::cout << "  " << r << std::endl;
	return true;
}

bool command_partners(Parser &parse) {
	Nid t1;

	if (!parse(value_<Nid>{t1})) {
		parse.syntax_error("'%partners' needs a node id");
		return true;
	}
	if (!parse(';')) {
		parse.warning("Expected ';'");
	}
	auto part = dharc::partners(t1);
	for (auto i : part) {
		cout << "  - " << i << std::endl;
	}
	return true;
}

void command_define(const vector<string> &e) {
	// Just a constant definition
	if (e.size() == 4) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		Nid h = Nid::from_string(e[3]);
		dharc::define(t1, t2, h);

	// Path based definition
	} else if (e.size() > 4) {
		Nid t1 = Nid::from_string(e[1]);
		Nid t2 = Nid::from_string(e[2]);
		dharc::define(t1, t2, build_path(e, 3));

	} else {
		cout << "  Define command expects 3 or more arguments." << std::endl;
	}
}

/*
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
/*map<string, void (*)(const vector<string>&)> commands = {
		{ "%query", command_query },
		{ "%define", command_define },
		{ "%partners", command_partners },
		// { "%path", command_path },
		// { "%array", command_array },
		// { "%dependants", command_dependants },
		// { "%details", command_details }
		// %string
		// %sigpath
		// %dependants
		// %dependencies
		// %definition
};*/

/*Nid parse_dsbscript(const vector<string> &tokens) {
	Nid cur = Nid::from_string(tokens[0]);
	Nid other;
	Nid old;

	for (int i = 1; i < static_cast<int>(tokens.size()); ++i) {
		if (tokens[i] == "=") {
			if (tokens[i+1] == "{") {
				++i;
				dharc::define(old, other, build_path(tokens, ++i));
				++i;
				cur = old;
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
}*/

void execute(std::istream &ss, const char *source) {
	Parser parse(ss);
	string cmd;

	while (!parse.eof()) {
		parse('%', [](auto &parse) {
			if (!(
				parse(word_{"query"}, command_query)
				|| parse(word_{"partners"}, command_partners)
			)) {
				parse.syntax_error("Unrecognised command");
				return false;
			}
			return true;
		});

		if (!parse.messages.empty()) {
			parse.print_messages(source);
			parse.skip_line();
		}
	}

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
		line += "\n";
		if (line.size() == 0) continue;
		std::stringstream ss(line);
		execute(ss, "stdin");
	}
}

static struct {
	int interactive;
} config {
	0,
};

static option opts[] = {
	{"interactive", 0, &config.interactive, 1},
	{"cmd", 1, nullptr, 'c'},
	{"port", 1, nullptr, 'p'},
	{"host", 1, nullptr, 'h'},
	{"file", 1, nullptr, 'f'},
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

