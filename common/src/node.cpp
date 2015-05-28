/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/node.hpp"

#include <atomic>
#include <ostream>
#include <string>
#include <sstream>
#include <iostream>

#include "dharc/parse.hpp"

using dharc::Node;
using dharc::parser::Context;
using std::string;
using std::stringstream;
using std::istream;
using std::ostream;

using dharc::parser::value;
using dharc::parser::word;

/* std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique() {
	return {Nid::Type::allocated, last_nid++};
}*/ 

Node::Node(const string &str) {
	stringstream ss(str);
	ss >> *this;
}

Node::Node(const char *str) {
	stringstream ss(str);
	ss >> *this;
}

Node::operator string() const {
	stringstream ss;
	ss << *this;
	return ss.str();
}

Node::operator int() const {
	switch(t) {
	case Type::integer : return static_cast<int>(i);
	default            : return 0;
	}
}

istream &dharc::operator>>(istream &is, Node &n) {
	n = null_n;

	int dummy = 0;
	Context parse(is);

	if (!( parse(value<int>{dummy}, ':', value<int64_t>{n.i},
			[&]() { n.t = static_cast<Node::Type>(dummy); })
		|| parse('\'', value<char>{n.c}, '\'',
			[&]() { n.t = Node::Type::character; })
		|| parse(value<double>{n.d},
			[&]() { n.t = Node::Type::real; })
		|| parse(value<int64_t>{n.i},
			[&]() { n.t = Node::Type::integer; })
		|| parse(word{"$"},
			[&]() { n = any_n; })
		|| parse(word{"true"},
			[&]() { n = true_n; })
		|| parse(word{"false"},
			[&]() { n = false_n; })
		|| parse(word{"null"},
			[&]() { n = null_n; }))) {
		// Mark stream as failed.
		is.clear(istream::failbit);
	}
	return is;
}

ostream &dharc::operator<<(ostream &os, const Node &n) {
	switch(n.t) {
	case Node::Type::special    :
		switch(n.i) {
		case kNullNode   : os << "null";   break;
		case kTrueNode   : os << "true";   break;
		case kFalseNode  : os << "false";  break;
		case kAnyNode    : os << "$";      break;
		}
		break;
	case Node::Type::integer    : os << n.i;                 break;
	case Node::Type::real       : os << n.d;                 break;
	case Node::Type::character  : os << '\'' << n.c << '\''; break;
	default : os << static_cast<int>(n.t) << ':' << n.i;
	}
	return os;
}

void dharc::rpc::Packer<Node>::pack(ostream &os, const Node &n) {
	os << '"' << static_cast<int>(n.t) << ':' << static_cast<int>(n.i) << '"';
}

Node dharc::rpc::Packer<Node>::unpack(istream &is) {
	Node res;
	int type;
	if (is.get() != '"') return null_n;
	is >> type;
	res.t = static_cast<Node::Type>(type);
	if (is.get() != ':') return null_n;
	is >> res.i;
	if (is.get() != '"') return null_n;
	return res;
}

