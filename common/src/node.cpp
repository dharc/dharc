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

istream &dharc::operator>>(istream &is, Node &n) {
	n = null_n;

	if (is.get() != '<') {
		is.clear(istream::failbit);
		return is;
	}

	is >> n.value;

	if (is.get() != '>') {
		is.clear(istream::failbit);		
	}
	return is;
}

ostream &dharc::operator<<(ostream &os, const Node &n) {
	os << '<' << n.value << '>';
	return os;
}

void dharc::rpc::Packer<Node>::pack(ostream &os, const Node &n) {
	os << '"' << n.value << '"';
}

Node dharc::rpc::Packer<Node>::unpack(istream &is) {
	Node res;
	if (is.get() != '"') return null_n;
	is >> res.value;
	if (is.get() != '"') return null_n;
	return res;
}

