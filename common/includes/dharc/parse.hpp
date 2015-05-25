/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PARSE_H_
#define DHARC_PARSE_H_

#include <istream>
#include <string>

namespace dharc {

struct Parser;

struct ws_ {
	bool operator()(Parser &ctx);
};

/**
 * Parse a variable number of items from the stream and return true if all
 * were parsed and false if not. If it failed to parse all items then the
 * stream is reset to its original read position. Items can be functors,
 * function pointers or lambdas. A lambda can be used at the end to act as
 * an action to take if successful, because it will only be called if
 * everything before it parsed.
 */
struct Parser {
	std::istream &stream;
	ws_ skip;

	public:
	Parser() = delete;
	explicit Parser(std::istream &s) : stream(s) {}
 
	template<typename... Args>
	bool operator()(Args... args) {
		std::streampos pos = stream.tellg();
		if (!parse_(args...) || stream.fail()) {
			stream.clear();
			stream.seekg(pos);
			return false;
		}
		return true;
	}

	private:
	/* Parse a string that matches str */
	bool parse__(const char *str);
	bool parse__(char c);

	/* Use a Functor for reading out values of certain types */
	template<typename T>
	bool parse__(T &t) {
		return t(*this);
	}

	/* Base nop case for no more arguments to be parsed */
	inline bool parse_() {
		return true;
	}

	/* Recursively parse the arguments. */
	template<typename F, typename... Args>
	bool parse_(F &first, Args&... args) {
		return skip(*this) && parse__(first) && parse_(args...);
	}
};


template<typename T>
struct value_ {
	T &value;
	bool operator()(Parser &ctx) {
		if (ctx.stream.eof()) return false;
		ctx.stream >> value;
		if (ctx.stream.fail()) {
			return false;
		}
		return true;
	}
};

struct word_ {
	const char *word;
	bool operator()(Parser &ctx);
};

struct id_ {
	std::string &id;
	bool operator()(Parser &ctx);
};

};  // namespace dharc

#endif  /* DHARC_PARSE_H_ */

