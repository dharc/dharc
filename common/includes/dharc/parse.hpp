/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PARSE_HPP_
#define DHARC_PARSE_HPP_

#include <istream>
#include <string>
#include <list>
#include <type_traits>

#include "dharc/node.hpp"

namespace dharc {
namespace parser {

struct Context;

/**
 * Functor to skip white space from a stream.
 */
struct skip {
	bool ccomments;
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
class Context {
	skip skip_;
	int lines_;
	bool noinfo_;
	std::list<Message> messages_;

	public:
	std::istream &stream;

	public:
	Context() = delete;
	explicit Context(std::istream &s) :
		skip_({true}), lines_(0), noinfo_(false), stream(s) {}

	void message(Message::Type type, const std::string &msg, int tag = 0);
	inline void syntaxError(const std::string &msg, int tag = 0);
	inline void runtimeError(const std::string &msg, int tag = 0);
	inline void warning(const std::string &msg, int tag = 0);
	void information(const std::string &msg, int tag = 0);

	const std::list<Message> &messages() const { return messages_; }

	void printMessages(const char *prefix = nullptr);

	bool eof();
	void skipLine();
	inline bool success() const { return !failed(); }
	bool failed() const;
	inline operator bool() const { return success(); }

	inline void showInformation(bool b) { noinfo_ = !b; }

	void reset();

	/**
	 * Main parse function. Sets up the parsing process and then calls the
	 * recursive template parse function to go through each argument.
	 */
	template<typename... Args>
	bool operator()(Args... args) {
		if (!startParse()) return false;
		// Save position incase of failure
		std::streampos pos = stream.tellg();
		int lcount = lines_;
		if (!parse_(args...) || stream.fail()) {
			stream.clear();
			// Reset because of failure
			stream.seekg(pos);
			lines_ = lcount;
			return false;
		}
		return true;
	}

	friend bool skip::operator()(Context &ctx);

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
		return skip_(*this) && parse__(first) && parse_(args...);
	}

	bool startParse();
};

/* Implementations of inline functions */
inline void Context::syntaxError(const std::string &msg, int tag) {
	message(Message::Type::syntax_error, msg, tag);
}
inline void Context::runtimeError(const std::string &msg, int tag) {
	message(Message::Type::runtime_error, msg, tag);
}
inline void Context::warning(const std::string &msg, int tag) {
	message(Message::Type::warning, msg, tag);
}


template<typename T>
struct value {
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
struct value<double> {
	double &value;
	bool operator()(Context &ctx);
};

struct word {
	const char *word;
	bool operator()(Context &ctx);
};

struct id {
	std::string &id;
	bool operator()(Context &ctx);

	static const int MAX_SIZE = 255;
};

typedef int token_t;

template<typename P>
struct token {
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

#endif  // DHARC_PARSE_HPP_

