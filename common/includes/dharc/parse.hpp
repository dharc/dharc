/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PARSE_H_
#define DHARC_PARSE_H_

#include <istream>
#include <string>
#include <list>

namespace dharc {

struct Parser;

struct skip_ {
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
	skip_ skip;
	int lines;
	
	struct Message {
		enum struct Type {
			warning,
			information,
			syntax_error,
			runtime_error
		};

		Type type;
		std::string message;
		std::string snapshot;
		int pos;
		int line;
		int tag;
	};

	std::list<Message> messages;

	public:
	Parser() = delete;
	explicit Parser(std::istream &s) : stream(s), lines(0) {}

	void message(Message::Type type, const std::string &msg, int tag=0);

	inline void syntax_error(const std::string &msg, int tag=0) {
		message(Message::Type::syntax_error, msg, tag);
	}
	inline void runtime_error(const std::string &msg, int tag=0) {
		message(Message::Type::runtime_error, msg, tag);
	}
	inline void warning(const std::string &msg, int tag=0) {
		message(Message::Type::warning, msg, tag);
	}
	inline void info(const std::string &msg, int tag=0) {
		message(Message::Type::information, msg, tag);
	}

	void print_messages(const char *prefix=nullptr);
 
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

typedef int token_t;

template<typename P>
struct token_ {
	P match;
	token_t &dest;
	token_t value;

	bool operator()(Parser &ctx) {
		if (match(ctx)) {
			dest = value;
			return true;
		}
		return false;
	}
};

};  // namespace dharc

#endif  /* DHARC_PARSE_H_ */

