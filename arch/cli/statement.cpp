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



void Statement::enter() {
	is_lhs_ = true;
	current_token_ = 0;
	token_position_ = lhs_tokens_[0].size();
	::move(getcury(stdscr), lhs_tokens_[current_token_].size() + 2);
	refresh();
}



void Statement::exit() {

}



void Statement::moveRight() {
	if (is_lhs_) {
		++current_token_;
		if (current_token_ == static_cast<int>(lhs_tokens_.size())) {
			current_token_ = 0;
			is_lhs_ = false;
			::move(getcury(stdscr), getcurx(stdscr)+3+
					rhs_tokens_[0].size());
			token_position_ = rhs_tokens_[0].size();
			return;
		} else {
			token_position_ = lhs_tokens_[current_token_].size();
			::move(getcury(stdscr), getcurx(stdscr)+
				lhs_tokens_[current_token_].size() + 1);
		}
	} else {
		++current_token_;
		if (current_token_ == static_cast<int>(rhs_tokens_.size())) {
			--current_token_;
			return;
		} else {
			::move(getcury(stdscr), getcurx(stdscr)+
				rhs_tokens_[current_token_].size() + 1);
			token_position_ = rhs_tokens_[current_token_].size();
		}
	}
}



void Statement::moveLeft() {
	if (is_lhs_) {
		--current_token_;

		if (current_token_ < 0) {
			current_token_ = 0;
			return;
		} else {
			token_position_ = lhs_tokens_[current_token_].size();
			::move(getcury(stdscr), getcurx(stdscr)-
				lhs_tokens_[current_token_ + 1].size() - 1);
		}
	} else {
		--current_token_;
		if (current_token_ < 0) {
			is_lhs_ = true;
			current_token_ = lhs_tokens_.size()-1;
			token_position_ = lhs_tokens_[current_token_].size();
			::move(getcury(stdscr), getcurx(stdscr)-
				rhs_tokens_[0].size() - 3);
		} else {
			token_position_ = rhs_tokens_[current_token_].size();
			::move(getcury(stdscr), getcurx(stdscr)-
				rhs_tokens_[current_token_ + 1].size() - 1);
		}
	}
}



void Statement::moveToRhs() {
	is_lhs_ = false;
	current_token_ = 0;
	token_position_ = 0;
	::move(getcury(stdscr), getcurx(stdscr)+3);
}



void Statement::insertToken() {
	// Don't allow empty tokens
	if (is_lhs_) {
		if (lhs_tokens_[current_token_].size() == 0) return;
	} else {
		if (rhs_tokens_[current_token_].size() == 0) return;
	}
	++current_token_;
	if (is_lhs_) {
		lhs_tokens_.insert(lhs_tokens_.begin()+current_token_, "");
		//if (static_cast<int>(lhs_tokens_.size()) == current_token_) {
		//	lhs_tokens_.push_back("");
		//}
	} else {
		//if (static_cast<int>(rhs_tokens_.size()) == current_token_) {
		//	rhs_tokens_.push_back("");
		//}
		rhs_tokens_.insert(rhs_tokens_.begin()+current_token_, "");
	}
	token_position_ = 0;
	::move(getcury(stdscr), getcurx(stdscr)+1);
}



void Statement::insertCurrent(int ch) {
	if (is_lhs_) {
		lhs_tokens_[current_token_].insert(token_position_, 1, ch);
		++token_position_;
	} else {
		rhs_tokens_[current_token_].insert(token_position_, 1, ch);
		++token_position_;
	}
	::move(getcury(stdscr), getcurx(stdscr)+1);
}



void Statement::deleteChar() {
	if (is_lhs_) {
		if (lhs_tokens_[current_token_].size() > 0) {
			lhs_tokens_[current_token_].pop_back();
			--token_position_;
			::move(getcury(stdscr), getcurx(stdscr)-1);
		}
	} else {
		if (rhs_tokens_[current_token_].size() > 0) {
			rhs_tokens_[current_token_].pop_back();
			--token_position_;
			::move(getcury(stdscr), getcurx(stdscr)-1);
		}
	}
}



void Statement::insertBracket(int ch) {
	if (is_lhs_) {
		if (lhs_tokens_[current_token_].size() != 0) {
			insertToken();
		}
	} else {
		if (rhs_tokens_[current_token_].size() != 0) {
			insertToken();
		}
	}
	insertCurrent(ch);
	insertToken();
}



void Statement::insert(int ch) {
	switch (ch) {
	case '('           :
	case ')'           :
	case '{'           :
	case '}'           : insertBracket(ch);                 break;
	case KEY_LEFT      : moveLeft();                        break;
	case KEY_RIGHT     : moveRight();                       break;
	case KEY_BACKSPACE : deleteChar();                      break;
	case '='           : moveToRhs();                       break;
	case ' '           : insertToken();                     break;
	default            : insertCurrent(ch);
	}
}

void Statement::position(int absolute) {

}

int Statement::position() {
	return 0;
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
	int ix = 0;

	::move(line, 0);
	::clrtoeol();
	::attron(A_BOLD);
	::addstr("> ");
	::attroff(A_BOLD);

	if (isEmpty()) return;

	ix = 0;
	for (auto i : lhs_tokens_) {
		Node n = Node(i);
		if (n == dharc::null_n) {
			if (dharc::labels.exists(i)) {
				n = Node(Node::Type::constant, 0);
			}
		}

		if (is_lhs_ && ix == current_token_) {
			::attron(A_UNDERLINE);
		}

		syntaxOn(n);
		::addstr(i.c_str());
		syntaxOff(n);

		if (is_lhs_ && ix == current_token_) {
			::attroff(A_UNDERLINE);
		}

		::addch(' ');

		++ix;
	}

	::addstr("= ");

	ix = 0;
	for (auto i : rhs_tokens_) {
		Node n = Node(i);
		if (n == dharc::null_n) {
			if (dharc::labels.exists(i)) {
				n = Node(Node::Type::constant, 0);
			}
		}

		if (!is_lhs_ && ix == current_token_) {
			::attron(A_UNDERLINE);
		}

		syntaxOn(n);
		::addstr(i.c_str());
		syntaxOff(n);

		if (!is_lhs_ && ix == current_token_) {
			::attroff(A_UNDERLINE);
		}

		::addch(' ');

		++ix;
	}

	::move(getcury(stdscr), getcurx(stdscr)-1);
	::addch(';');

	if (rhs_tokens_[0] == "{") {
		::attron(COLOR_PAIR(3));
		::addstr("  // == ");
		if (result_.t == Node::Type::constant) {
			::addstr(dharc::labels.get(result_.i).c_str());
		} else {
			::addstr(((string)result_).c_str());
		}
		::attroff(COLOR_PAIR(3));
	}

	if (error_) {
		int window_width;
		window_width = getmaxx(stdscr);

		window_width -= error_msg_.message.size() + 10;
		::move(getcury(stdscr), window_width);

		::attron(A_BOLD);
		::addch('[');
		::attron(COLOR_PAIR(1));
		::addstr("error: ");
		::attroff(COLOR_PAIR(1));
		::addstr(error_msg_.message.c_str());
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

		error_ = false;

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
	if (error_) return true;

	Node oldres = result_;
	result_ = executeLhs();

	if (oldres != result_) {
		if (rhs_tokens_[0] != "{") {
			if (result_.t == Node::Type::constant) {
				rhs_tokens_[0] = dharc::labels.get(result_.i);
			} else {
				rhs_tokens_[0] = (string)result_;
			}
		}
		return true;
	}
	return false;
}

