/*
 * Copyright 2015 Nicolas Pope
 */

#include <getopt.h>
#include <csignal>
#include <ncurses.h>
#include <cstring>

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "dharc/node.hpp"
#include "dharc/arch.hpp"
#include "dharc/script.hpp"
#include "dharc/labels.hpp"

#include "statement.hpp"

using dharc::Statement;

using dharc::Node;
using dharc::arch::Script;
using dharc::labels;

using std::map;
using std::string;
using std::vector;
using std::stringstream;
using std::cout;

int window_width = 0;
int window_height = 0;

static struct {
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


static void execute(std::istream &ss, const char *source) {
	Script script(ss, source);

	if (config.no_info) {
		script.showInformation(false);
	} else {
		script.showInformation(true);
	}

	Node res = script(config.params);

	if (script.hasResult()) {
		string result;

		if (res.t == Node::Type::constant) {
			string label = labels.get(res.i);
			if (label == "NAL") {
				result = (string)res;
			} else {
				result = label;
			}
		} else {
			result = (string)res;
		}

		if (config.interactive) {
			printw("    %s\n",result.c_str());
		} else {
			std::cout << res << std::endl;
		}
	}
}

vector<string> get_suggestions(const char *line) {
	string command(line);
	string partial;
	vector<string> result;

	// Remove incomplete word.
	while (!command.empty() && command.back() != ' ') {
		partial.push_back(command.back());
		command.pop_back();
	}
	std::reverse(partial.begin(), partial.end());

	// Remove white space
	while (!command.empty() && command.back() == ' ') command.pop_back();

	// if not an identifier return
	
	// evaluate truncated string
	command += ';';
	std::stringstream ss(command);
	Script script(ss, "");
	Node res = script(config.params);
	if (script.hasResult()) {
		list<Node> partners = dharc::partners(res);

		for (auto i : partners) {
			string istr;
			if (i.t == Node::Type::constant) {
				istr = dharc::labels.get(i.i);
			} else {
				istr = (string)i;
			}
			if (istr.find(partial) == 0) {
				result.push_back(istr);
			}
		}
	}

	return result;
}

void interactive() {
	vector<Statement> statements;
	statements.push_back(Statement(""));
	int current = 0;
	int ix = 0;

	statements[current].display(0);
	::move(0, 2);

	while (true) {
		int ch = getch();

		switch (ch) {
		case KEY_UP:
			statements[current].execute();
			statements[current].display(current);

			ix = 0;
			for (auto i : statements) {
				if (i.refresh()) {
					i.display(ix);
				}
				++ix;
			}

			if (current != 0) {
				--current;
				::move(current, 2);
				statements[current].enter();
				statements[current].display(current);
			} else {
				::move(0, 2);
			}
			break;
		case KEY_DOWN:
		case '\n' :
			statements[current].execute();
			statements[current].display(current);

			ix = 0;
			for (auto i : statements) {
				if (i.refresh()) {
					i.display(ix);
				}
				++ix;
			}

			++current;
			if (static_cast<int>(statements.size()) == current) {
				statements.push_back(Statement(""));
			}
			::move(getcury(stdscr)+1, 2);
			statements[current].enter();
			statements[current].display(current);
			break;
		default:
			if ((ch == KEY_BACKSPACE) && statements[current].isEmpty()) {
				if (current > 0) {
					::move(current, 0);
					::clrtobot();
					statements.erase(statements.begin()+current);
					--current;
					::move(current, 2);

					ix = 0;
					for (auto i : statements) {
						//if (i.refresh()) {
							i.display(ix);
						//}
						++ix;
					}
				}
			} else {
				statements[current].insert(ch);
				statements[current].display(current);
			}
		}
	}
}

/*
 * Read command line entries when in interactive mode and pass to relevant
 * command function for processing.
 */
/*static void interactive() {
	vector<string> history;
	char line[300];
	int position = 0;
	int historyix = 0;
	vector<string> suggestions;

	move(window_height-1, 0);

	int ch = 0;

	attron(A_BOLD);
	addstr("> ");
	attroff(A_BOLD);

	while (true) {
	ch = getch();

	switch(ch) {
	case KEY_BACKSPACE :
		if (position == 0) break;
		line[--position] = 0;
		move(window_height-1, getcurx(stdscr)-1);
		addch(' ');
		move(window_height-1, getcurx(stdscr)-1);
		break;
	case KEY_UP        :
		if (historyix > 0) {
			--historyix;
			move(window_height-1, 2);
			clrtoeol();
			strcpy(line, history[historyix].c_str());
			position = strlen(line);
			addstr(line);
		}
		break;
	case KEY_DOWN      :
		if (historyix < static_cast<int>(history.size())) {
			++historyix;
			if (historyix == static_cast<int>(history.size())) {
				position = 0;
				line[0] = 0;
				move(window_height-1, 2);
				clrtoeol();
			} else {
				move(window_height-1, 2);
				clrtoeol();
				strcpy(line, history[historyix].c_str());
				position = strlen(line);
				addstr(line);
			}
		}
		break;
	case KEY_LEFT      : break;
	case KEY_RIGHT     : break;
	case KEY_HOME      : break;
	case '\t'          :
		suggestions = get_suggestions(line);
		if (suggestions.size() > 1) {
			move(window_height-2, window_width-1);
			addch('\n');
			attron(COLOR_PAIR(1));
			clrtoeol();
			addstr("\n  --> ");
			for (auto i : suggestions) {
				printw("%s ", i.c_str());
			}
			attroff(COLOR_PAIR(1));
			addch('\n');
			move(window_height-1, 0);
			printw("> %s", line);
		} else if (suggestions.size() == 1) {
			// append remainder of the word to line.
			while (line[position] != ' ') --position;
			strcpy(&line[position+1], suggestions[0].c_str());
			position += suggestions[0].size()+1;
			line[position] = 0;
			move(window_height-1, 0);
			printw("> %s", line);
		}
		break;
	case '\n':
		if (position == 0) {
			attron(A_BOLD);
			addstr("\n> ");
			attroff(A_BOLD);
		} else if (line[position-1] == ';') {
			history.push_back(line);
			historyix = history.size();
			addch('\n');
			std::stringstream ss(line);
			execute(ss, "stdin");
			addch('\n');
			position = 0;
			line[0] = 0;
			
			move(window_height-1, 0);
			attron(A_BOLD);
			addstr("> ");
			attroff(A_BOLD);
		} else {
			addch('\n');
			line[position++] = '\n';
			line[position] = 0;
			move(window_height-1, 2);
		}
		break;
	default            :
		line[position++] = ch;
		line[position] = 0;
		addch(ch);
		// move(window_height-1, getcurx(stdscr)+1);
	}
	}

}*/

void signal_handler(int param) {
	if (config.no_labels == 0) {
		std::ofstream labelout;
		labelout.open("./.labels.dharc",
			std::ofstream::out | std::ofstream::trunc);
		labels.dump(labelout);
		labelout.close();
	}

	std::cout << std::endl;

	dharc::stop();

	endwin();
	exit(0);
}

static option opts[] = {
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

int main(int argc, char *argv[]) {
	int o;
	std::stringstream ss;
	std::ifstream ifs;

	signal(SIGINT, signal_handler);

	// Initialise ncurses
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	scrollok(stdscr, TRUE);
	getmaxyx(stdscr, window_height, window_width);

	dharc::start(argc, argv);

	while ((o = getopt_long(argc, argv, "f:ic:h:p:", opts, nullptr)) != -1) {
		switch(o) {
		case 'c': ss.str(string(optarg)); execute(ss, "cli"); break;
		case 'i': config.interactive = 1; break;
		case 'h': break;
		case 'p': break;
		case 1000: std::cout << helpmsg; break;
		case '*': config.params.push_back(Node(optarg)); break;
		case 'f': ifs.open(optarg); execute(ifs, optarg); ifs.close(); break;
		case ':': cout << "Option '" << optopt << "' requires an argument\n";
					break;
		default: break;
		}
	}

	if (config.interactive) {
		interactive();
	}

	dharc::stop();

	// Finish ncurses
	endwin();

	return 0;
}

