#include "statement.hpp"

#include <ncurses.h>
#include <cstring>
#include <sstream>

#include "dharc/node.hpp"
#include "dharc/script.hpp"
#include "dharc/labels.hpp"

using dharc::Statement;
using dharc::Node;
using dharc::arch::Script;

Statement::Statement(string str) {
	lhs_tokens_.push_back("");
	rhs_tokens_.push_back("");
	current_token_ = 0;
	is_lhs_ = true;
	token_position_ = 0;
	error_ = false;
}

Statement::~Statement() {

}

void Statement::insert(int ch) {
	switch (ch) {
	case KEY_LEFT : break;
	case KEY_RIGHT :
		if (is_lhs_) {
			++token_position_;
			if (token_position_ > static_cast<int>(lhs_tokens_[current_token_].size())) {
				token_position_ = 0;
				++current_token_;
				if (current_token_ == static_cast<int>(lhs_tokens_.size())) {
					current_token_ = 0;
					is_lhs_ = false;
					::move(getcury(stdscr), getcurx(stdscr)+3);
					break;
				}
			}
			::move(getcury(stdscr), getcurx(stdscr)+1);
		} else {
			++token_position_;
			if (token_position_ > static_cast<int>(rhs_tokens_[current_token_].size())) {
				token_position_ = 0;
				++current_token_;
				if (current_token_ == static_cast<int>(rhs_tokens_.size())) {
					--current_token_;
					break;
				}
			}
			::move(getcury(stdscr), getcurx(stdscr)+1);
		}
		break;
	case KEY_BACKSPACE : break;
	case '=':
		is_lhs_ = false;
		current_token_ = 0;
		token_position_ = 0;
		::move(getcury(stdscr), getcurx(stdscr)+3);
		break;
	case ' ' :
		// Don't allow empty tokens
		if (is_lhs_) {
			if (lhs_tokens_[current_token_].size() == 0) break;
		} else {
			if (rhs_tokens_[current_token_].size() == 0) break;
		}
		++current_token_;
		if (is_lhs_) {
			if (static_cast<int>(lhs_tokens_.size()) == current_token_) {
				lhs_tokens_.push_back("");
			}
		} else {
			if (static_cast<int>(rhs_tokens_.size()) == current_token_) {
				rhs_tokens_.push_back("");
			}
		}
		token_position_ = 0;
		::move(getcury(stdscr), getcurx(stdscr)+1);
		break;
	default  :
		if (is_lhs_) {
			lhs_tokens_[current_token_].insert(token_position_, 1, ch);
			++token_position_;
		} else {
			rhs_tokens_[current_token_].insert(token_position_, 1, ch);
			++token_position_;
		}
		::move(getcury(stdscr), getcurx(stdscr)+1);
	}
}

void Statement::position(int absolute) {

}

int Statement::position() {
	return 0;
}

void Statement::resetPosition() {
	is_lhs_ = true;
	current_token_ = 0;
	token_position_ = 0;
}

void Statement::move(int relative) {

}

void syntaxOn(const Node &n) {
	if (n.t == Node::Type::integer) {
		::attron(COLOR_PAIR(2));
	} else if (n.t == Node::Type::special && n.i != 0) {
		::attron(A_BOLD);
	} else if (n == dharc::null_n) {
		::attron(COLOR_PAIR(1));
	}
}

void syntaxOff(const Node &n) {
	if (n.t == Node::Type::integer) {
		::attroff(COLOR_PAIR(2));
	} else if (n.t == Node::Type::special && n.i != 0) {
		::attroff(A_BOLD);
	} else if (n == dharc::null_n) {
		::attroff(COLOR_PAIR(1));
	}
}

bool Statement::isEmpty() {
	return (lhs_tokens_.size() == 1 &&
		lhs_tokens_[0].size() == 0 &&
		rhs_tokens_.size() == 1 &&
		rhs_tokens_[0].size() == 0);
}

void Statement::display(int line) {
	int x = getcurx(stdscr);

	::move(line, 0);
	::clrtoeol();
	::attron(A_BOLD);
	::addstr("> ");
	::attroff(A_BOLD);

	if (isEmpty()) return;

	for (auto i : lhs_tokens_) {
		Node n = Node(i);
		if (n == dharc::null_n) {
			if (dharc::labels.exists(i)) {
				n = Node(Node::Type::constant, 0);
			}
		}
		syntaxOn(n);
		::addstr(i.c_str());
		syntaxOff(n);
		::addch(' ');
	}

	::addstr("= ");

	for (auto i : rhs_tokens_) {
		Node n = Node(i);
		syntaxOn(n);
		::addstr(i.c_str());
		syntaxOff(n);
		::addch(' ');
	}

	::addch(';');

	if (error_) {
		::attron(A_BOLD);
		addstr("    [");
		::attron(COLOR_PAIR(1));
		::addstr(error_msg_.message.c_str());
		::attroff(COLOR_PAIR(1));
		::addch(']');
		::attroff(A_BOLD);
	}

	::move(line, x);
}

Node Statement::executeLhs() {
	if (isEmpty()) return dharc::null_n;

	std::stringstream ss;

	for (auto i : lhs_tokens_) {
		ss << i << " ";
	}
	ss << ";";

	Script script(ss, "");
	script.showMessages(false);
	return script({});
}

void Statement::execute() {
	if (isEmpty()) return;

	if (rhs_tokens_.size() > 1 || rhs_tokens_[0].size() > 0) {
		std::stringstream ss;

		for (auto i : lhs_tokens_) {
			ss << i << " ";
		}
		ss << " = ";
		for (auto i : rhs_tokens_) {
			ss << i << " ";
		}
		ss << ";";

		Script script(ss, "");
		script.showMessages(false);
		script({});

		if (script.messages().empty() == false) {
			error_ = true;
			error_msg_ = script.messages().front();
		}
	}

	// refresh();
}

bool Statement::refresh() {
	Node oldres = result_;
	result_ = executeLhs();

	if (oldres != result_) {
		rhs_tokens_[0] = (string)result_;
		return true;
	}
	return false;
}

