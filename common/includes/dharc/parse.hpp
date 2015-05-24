/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PARSE_H_
#define DHARC_PARSE_H_

#include <istream>

namespace dharc {

template<typename T>
struct Token {
	T &value;
	bool parse(std::istream &is);
};

/* ==== INTERNALS =========================================================== */
namespace intern {

/* Parse a string that matches str */
bool parse__(std::istream &is, const char *str);

/* Use a parse structure for reading out values of certain types */
template<typename T>
bool parse__(std::istream &is, T t) {
	return t.parse(is);
}

/* Base nop case for no more arguments to be parsed */
constexpr bool parse_(std::istream &is) {
	return true;
}

/* Recursively parse the arguments. */
template<typename F, typename... Args>
bool parse_(std::istream &is, F first, Args... args) {
	return parse__(is, first) && parse_(is, args...);
}

};  // namespace intern
/* ========================================================================== */

/**
 * Parse a variable number of items from the stream and return true if all
 * were parsed and false if not. If it failed to parse all items then the
 * stream is reset to its original read position.
 */
template<typename... Args>
bool parse(std::istream &is, Args... args) {
	std::streampos pos = is.tellg();
	if (intern::parse_(is, args...)) {
		return true;
	} else {
		is.seekg(pos);
		return false;
	}
}

};  // namespace dharc

#endif  /* DHARC_PARSE_H_ */

