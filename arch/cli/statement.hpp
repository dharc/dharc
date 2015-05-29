/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_STATEMENT_HPP_
#define DHARC_STATEMENT_HPP_

#include <string>
#include <vector>

#include "dharc/parse.hpp"
#include "dharc/node.hpp"

using std::vector;
using std::string;

namespace dharc {

class Statement {
	public:
	Statement(string str);
	~Statement();

	void insert(int ch);
	void position(int absolute);
	int position();
	void move(int relative);

	void display(int line);

	void execute();
	Node executeLhs();
	bool refresh();

	void enter();
	void exit();

	bool isEmpty();

	private:
	vector<string> lhs_tokens_;
	vector<string> rhs_tokens_;
	bool error_;
	dharc::parser::Message error_msg_;
	bool is_lhs_;
	int current_token_;
	int token_position_;
	Node result_;

	void moveLeft();
	void moveRight();
	void insertCurrent(int ch);
	void moveToRhs();
	void insertToken();
	void insertBracket(int ch);
	void movePrev();
	void deleteChar();
};

};  // namespace dharc

#endif  // DHARC_STATEMENT_HPP_

