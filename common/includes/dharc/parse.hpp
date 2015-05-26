/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PARSE_H_
#define DHARC_PARSE_H_

#include <istream>
#include <string>
#include <list>
#include <type_traits>

#include "dharc/nid.hpp"

namespace dharc {
namespace parser {

struct Context;

struct skip_ {
	bool operator()(Context &ctx);
};

/* Nop actor to terminate parse */
inline void noact() {}

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

/**
 * Parse a variable number of items from the stream and return true if all
 * were parsed and false if not. If it failed to parse all items then the
 * stream is reset to its original read position. Items can be functors,
 * function pointers or lambdas. A lambda can be used at the end to act as
 * an action to take if successful, because it will only be called if
 * everything before it parsed.
 */
struct Context {
	std::istream &stream;
	skip_ skip;
	int lines;
	bool noinfo;

	std::list<Message> messages;

	public:
	Context() = delete;
	explicit Context(std::istream &s) : stream(s), lines(0), noinfo(false) {}

	void message(Message::Type type, const std::string &msg, int tag = 0);

	inline void syntax_error(const std::string &msg, int tag = 0) {
		message(Message::Type::syntax_error, msg, tag);
	}
	inline void runtime_error(const std::string &msg, int tag = 0) {
		message(Message::Type::runtime_error, msg, tag);
	}
	inline void warning(const std::string &msg, int tag = 0) {
		message(Message::Type::warning, msg, tag);
	}
	void info(const std::string &msg, int tag = 0);

	void print_messages(const char *prefix = nullptr);

	bool eof();
	void skip_line();

	inline bool success() const { return !failed(); }
	bool failed() const;
	inline operator bool() const { return success(); }

	inline void show_info(bool b) { noinfo = !b; }

	void reset();

	/**
	 * Main parse function. Sets up the parsing process and then calls the
	 * recursive template parse function to go through each argument.
	 */
	template<typename... Args>
	bool operator()(Args... args) {
		if (!start_parse()) return false;
		// Save position incase of failure
		std::streampos pos = stream.tellg();
		int lcount = lines;
		if (!parse_(args...) || stream.fail()) {
			stream.clear();
			// Reset because of failure
			stream.seekg(pos);
			lines = lcount;
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

	/* Base case, call the actor */
	template<typename T>
	bool parse_(T &t) {
		static_assert(std::is_void<typename std::result_of<T()>::type>::value,
			"Parse actor had non-void return type");
		t();
		return true;
	}

	/* Recursively parse the arguments. */
	template<typename F, typename... Args>
	bool parse_(F &first, Args&... args) {
		return skip(*this) && parse__(first) && parse_(args...);
	}

	bool start_parse();
};


template<typename T>
struct value_ {
	T &value;
	bool operator()(Context &ctx) {
		if (ctx.stream.eof()) return false;
		ctx.stream >> value;
		if (ctx.stream.fail()) {
			return false;
		}
		return true;
	}
};

template<>
bool value_<Nid>::operator()(Context &ctx);

struct word_ {
	const char *word;
	bool operator()(Context &ctx);
};

struct id_ {
	std::string &id;
	bool operator()(Context &ctx);
};

typedef int token_t;

template<typename P>
struct token_ {
	P match;
	token_t &dest;
	token_t value;

	bool operator()(Context &ctx) {
		if (match(ctx)) {
			dest = value;
			return true;
		}
		return false;
	}
};

};  // namespace parser
};  // namespace dharc

#endif  /* DHARC_PARSE_H_ */

