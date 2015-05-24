/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/parse.hpp"

#include <istream>
#include <string>

using std::istream;
using std::string;

bool dharc::intern::parse__(istream &is, const char *str) {
	char buf[100];
	string cppstr(str);
	is.read(buf, cppstr.size());
	buf[cppstr.size()] = 0;
	return (cppstr == buf);
}

namespace dharc {
template<>
bool Token<int>::parse(std::istream &is) {
	is >> value;
	return true;
}
};

