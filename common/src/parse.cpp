/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/parse.hpp"

#include <iostream>
#include <string>
#include <cstring>

using std::istream;
using std::string;
using dharc::Parser;
using std::cout;

bool dharc::Parser::parse__(const char *str) {
	char buf[100];
	string cppstr(str);
	if (stream.eof()) return false;
	stream.read(buf, cppstr.size());
	if (stream.fail()) {
		return false;
	}
	buf[cppstr.size()] = 0;
	return (cppstr == buf);
}

bool dharc::Parser::parse__(char c) {
	if (stream.eof()) return false;
	return stream.get() == c;
}

bool dharc::Parser::start_parse() {
	// skip(*this);
	if (!messages.empty()) messages.pop_back();
	if (stream.eof()) {
		return false;
	}
	return true;
}

bool dharc::Parser::eof() {
	skip(*this);
	return stream.eof();
}

void dharc::Parser::skip_line() {
	stream.ignore(100000,'\n');
	++lines;
}

constexpr bool is_alphanumeric(char c) {
	return (
		(c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9')
	);
}

#define GREEN "\033[0;32m"
#define RED "\033[0;31;1m"
#define YELLOW "\033[0;33;1m"
#define BOLDWHITE "\033[0;37;1m"
#define NOCOLOR "\033[0m"

void Parser::message(Message::Type type, const string &msg, int tag) {
	char snap[61];
	int pos = 0;
	//stream.clear();
	//skip(*this);

	if (stream.eof()) stream.seekg(-1, std::ios_base::cur);

	int start_pos = stream.tellg();

	while ((stream.tellg() > 0) && (pos < 40)) {
		stream.seekg(-1, std::ios_base::cur);
		++pos;
		if (stream.peek() == '\n') {
			stream.ignore(1);
			--pos;
			break;
		}
	}

	stream.get(snap, 60);

	string snapstr(snap);
	if (snapstr.length() >= 60) snapstr += "...";

	messages.push_back({type, msg, snapstr, pos, lines+1, tag});

	stream.seekg(start_pos);
}

void Parser::print_messages(const char *prefix) {
	for (auto i : messages) {
		if (prefix) cout << BOLDWHITE << prefix << NOCOLOR << ':';
		cout << BOLDWHITE << i.line << ": " << NOCOLOR;

		switch (i.type) {
		case Message::Type::syntax_error:
			cout << RED << "error: " << NOCOLOR; break;
		case Message::Type::runtime_error:
			cout << RED << "error: " << NOCOLOR; break;
		case Message::Type::warning:
			cout << YELLOW << "warn: " << NOCOLOR; break;
		case Message::Type::information:
			cout << "info: "; break;
		}

		int len = 1;
		string snapstr = i.snapshot;

		if (i.pos < static_cast<int>(snapstr.size())) {
			if (is_alphanumeric(snapstr.at(i.pos))) {
				while ((i.pos + len < static_cast<int>(snapstr.size()))
					&& is_alphanumeric(snapstr.at(i.pos+len))) ++len;
			} else {
				while ((i.pos + len < static_cast<int>(snapstr.size()))
					&& !is_alphanumeric(snapstr.at(i.pos+len))) ++len;
			}

			snapstr.insert(i.pos+len, NOCOLOR);
			snapstr.insert(i.pos, YELLOW);
		} else {
			snapstr += YELLOW "^" NOCOLOR;
		}

		cout << i.message;
		cout << std::endl;
		cout << "    " << snapstr;
		cout << std::endl;
	}
	messages.clear();
}

namespace dharc {

bool skip_::operator()(Parser &ctx) {
	char c;
	while (!ctx.stream.eof() && (c = ctx.stream.peek()) &&
		((c == ' ') || (c == '\t') || (c == '\n'))) {
		if (c == '\n') ++ctx.lines;
		ctx.stream.ignore(1);
	}
	return true;
}

bool word_::operator()(Parser &ctx) {
	char buf[101];
	size_t len = strlen(word);
	if (len >= 100) return false;
	if (ctx.stream.eof()) return false;
	ctx.stream.read(buf, len);
	if (ctx.stream.fail()) {
		return false;
	}
	char c = ctx.stream.peek();
	if (is_alphanumeric(c)) return false;
	buf[len] = 0;
	return (strncmp(buf,word,len) == 0);
}

bool id_::operator()(Parser &ctx) {
	char buf[101];
	int ix = 0;
	while (ix < 100 && !ctx.stream.eof() && (buf[ix] = ctx.stream.peek())) {
		if (is_alphanumeric(buf[ix]) || buf[ix] == '_') {
			++ix;
			ctx.stream.ignore(1);
		} else {
			break;
		}
	}
	buf[ix] = 0;
	if (ix == 0) return false;
	if (ix == 100) return false;
	if (buf[0] >= '0' && buf[0] <= '9') return false;
	id = buf;
	return true;
}

};

