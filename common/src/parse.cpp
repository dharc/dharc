/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/parse.hpp"

#include <istream>
#include <string>
#include <cstring>

using std::istream;
using std::string;

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

constexpr bool is_alphanumeric(char c) {
	return (
		(c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9')
	);
}

namespace dharc {

bool ws_::operator()(Parser &ctx) {
	char c;
	while (!ctx.stream.eof() && (c = ctx.stream.peek()) &&
		((c == ' ') || (c == '\t') || (c == '\n'))) {
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

