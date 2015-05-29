/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/parse.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <limits>

using std::istream;
using std::string;
using dharc::parser::Context;
using dharc::parser::Message;
using std::cout;
using std::cerr;

bool Context::parse__(const char *str) {
	if (stream.eof()) return false;

	auto buf = std::get_temporary_buffer<char>(strlen(str)+1);

	stream.read(buf.first, buf.second-1);
	if (stream.fail()) {
		std::return_temporary_buffer(buf.first);
		return false;
	}

	buf.first[buf.second-1] = 0;
	bool res = (strcmp(str, buf.first) == 0);

	std::return_temporary_buffer(buf.first);
	return res;
}



bool Context::parse__(char c) {
	if (stream.eof()) return false;
	return stream.get() == c;
}



bool Context::startParse() {
	if (stream.eof()) {
		return false;
	}
	return true;
}



bool Context::eof() {
	skip_(*this);
	return stream.eof();
}



void Context::skipLine() {
	stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	++lines_;
}



void Context::reset() {
	messages_.clear();
	lines_ = 0;
	stream.clear();
	stream.seekg(0);
}



bool Context::failed() const {
	for (auto i : messages_) {
		if ((i.type == Message::Type::syntax_error) ||
			(i.type == Message::Type::runtime_error)) {
			return true;
		}
	}
	return false;
}



constexpr bool isAlphaNumericX(char c) {
	return (
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9') ||
		(c == '_'));
}



constexpr bool isAlphaNumeric(char c) {
	return (
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9'));
}



constexpr bool isNumeric(char c) {
	return (c >= '0' && c <= '9');
}



constexpr bool isAlpha(char c) {
	return (
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z'));
}



constexpr bool isWhiteSpace(char c) {
	return (c == ' ' || c == '\t' || c == '\n');
}



void Context::message(Message::Type type, const string &msg, int tag) {
	char snap[61];
	int pos = 0;
	bool waseof = false;

	skip_(*this);

	if (stream.eof()) {
		stream.seekg(-1, std::ios_base::cur);
		waseof = true;
	}

	// Record position to restore at the end
	int start_pos = stream.tellg();

	// Move stream back to beginning of the line (or max 40 chars)
	while ((stream.tellg() > 0) && (pos < 40)) {
		stream.seekg(-1, std::ios_base::cur);
		++pos;
		if (stream.peek() == '\n') {
			stream.ignore(1);
			--pos;
			break;
		}
	}

	// Read the line, or 60 chars
	stream.get(snap, 60);

	// Append '...' if too long
	string snapstr(snap);
	if (snapstr.length() >= 60) snapstr += "...";

	// Restore
	if (waseof) {
		messages_.push_back({type, msg, snapstr, pos+1, lines_+1, tag});
		stream.seekg(start_pos+1);
	} else {
		messages_.push_back({type, msg, snapstr, pos, lines_+1, tag});
		stream.seekg(start_pos);
	}
}



void Context::information(const string &msg, int tag) {
	messages_.push_back({
		Message::Type::information, msg, "", 0, lines_+1, tag });
}



/* Terminal color codes */
constexpr auto GREEN = "\033[0;32m";
constexpr auto RED = "\033[0;31;1m";
constexpr auto YELLOW = "\033[0;33;1m";
constexpr auto BLUE = "\033[0;34;1m";
constexpr auto BOLDWHITE = "\033[0;37;1m";
constexpr auto NOCOLOR = "\033[0m";



void Context::printMessages(const char *prefix) {
	for (auto i : messages_) {
		// Skip unwanted information messages
		if (noinfo_ && i.type == Message::Type::information) continue;

		// Add bold prefix (filename...) and line number
		if (prefix) cerr << BOLDWHITE << prefix << NOCOLOR << ':';
		cerr << BOLDWHITE << i.line << ": " << NOCOLOR;

		// Print red errors, yellow warns and blue infos
		switch (i.type) {
		case Message::Type::syntax_error:
			cerr << RED << "error: " << NOCOLOR; break;
		case Message::Type::runtime_error:
			cerr << RED << "error: " << NOCOLOR; break;
		case Message::Type::warning:
			cerr << YELLOW << "warning: " << NOCOLOR; break;
		case Message::Type::information:
			cerr << BLUE << "info: " << NOCOLOR; break;
		}

		cerr << i.message << std::endl;

		if (i.snapshot.length() != 0) {
			int len = 1;
			string snapstr = i.snapshot;

			// Make the word/sequence nearest the error position yellow.
			if (i.pos < static_cast<int>(snapstr.size())) {
				// Highlight entire word/identifier
				if (isAlphaNumericX(snapstr.at(i.pos))) {
					while ((i.pos + len < static_cast<int>(snapstr.size())) &&
							isAlphaNumericX(snapstr.at(i.pos+len))) ++len;
				}

				snapstr.insert(i.pos+len, NOCOLOR);
				snapstr.insert(i.pos, YELLOW);
			} else {
				// End of line so add ^
				snapstr += YELLOW;
				snapstr += "^";
				snapstr += NOCOLOR;
			}

			cerr << "    " << snapstr;
			cerr << std::endl;
		}
	}
	messages_.clear();
}



namespace dharc {
namespace parser {

bool value<double>::operator()(Context &ctx) {
	if (ctx.stream.eof()) return false;

	char c;
	int pos = ctx.stream.tellg();

	// Scan for a dot
	while ((c = ctx.stream.get()) && isNumeric(c)) {}
	if (c != '.') return false;

	ctx.stream.seekg(pos);
	ctx.stream >> value;
	return !ctx.stream.fail();
}



bool skip::operator()(Context &ctx) {
	char c;
	while (!ctx.stream.eof() && (c = ctx.stream.peek())) {
		if (isWhiteSpace(c)) {
			if (c == '\n') ++ctx.lines_;
			ctx.stream.ignore(1);
		} else if (ccomments && c == '/') {
			ctx.stream.ignore(1);
			c = ctx.stream.peek();
			if (c == '/') {
				ctx.skipLine();
			} else if (c == '*') {
				ctx.stream.ignore(1);
				c = ctx.stream.get();
				char nc;
				while (!ctx.stream.eof() && (nc = ctx.stream.get())) {
					if (nc == '/' && c == '*') {
						break;
					} else {
						c = nc;
					}
				}
			} else {
				ctx.stream.unget();
				return true;
			}
		} else {
			return true;
		}
	}
	return true;
}



bool word::operator()(Context &ctx) {
	if (ctx.stream.eof()) return false;

	auto buffer = std::get_temporary_buffer<char>(strlen(word)+1);

	// Attempt to read the word from stream
	ctx.stream.get(buffer.first, buffer.second, 0);
	if (ctx.stream.fail()) {
		std::return_temporary_buffer(buffer.first);
		return false;
	}

	// Make sure it is the end of the word
	if (!ctx.stream.eof()) {
		char c = ctx.stream.peek();
		if (isAlphaNumericX(c)) {
			std::return_temporary_buffer(buffer.first);
			return false;
		}
	}

	bool result = (strcmp(buffer.first, word) == 0);
	std::return_temporary_buffer(buffer.first);
	return result;
}



bool id::operator()(Context &ctx) {
	if (ctx.stream.eof()) return false;

	auto buffer = std::get_temporary_buffer<char>(MAX_SIZE+1);

	int ix = 0;
	while (ix < (buffer.second - 1) && !ctx.stream.eof() &&
		(buffer.first[ix] = ctx.stream.peek())) {
		if (isAlphaNumericX(buffer.first[ix])) {
			++ix;
			ctx.stream.ignore(1);
		} else {
			break;
		}
	}

	buffer.first[ix] = 0;
	if (ix == 0 || isNumeric(buffer.first[0])) {
		std::return_temporary_buffer(buffer.first);
		return false;
	}

	id = buffer.first;

	std::return_temporary_buffer(buffer.first);
	return true;
}

};  // namespace parser
};  // namespace dharc

