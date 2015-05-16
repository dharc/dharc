/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/nid.hpp"

#include <atomic>
#include <ostream>
#include <string>

#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using fdsb::Nid;
using std::string;

std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique() {
	return {Nid::Type::allocated, last_nid++};
}

Nid Nid::from_string(const std::string &str) {
	Nid r = null_n;

	if (str.size() > 0) {
		if (str.at(0) >= '0' && str.at(0) <= '9') {
			if (str.find('.') == 1) {
				//Its a double
			} else {
				r.t = Nid::Type::integer;
				r.i = stoll(str);
			}
		} else if (str.at(0) == '\'') {
			r.t = Nid::Type::character;
			r.c = str.at(1);
		} else if (str.at(0) == '[') {
			string str2 = str.substr(1,str.size()-2);
			return from_string(str2);
		} else if (str == "true") {
			r = true_n;
		} else if (str == "false") {
			r = false_n;
		} else if (str == "null") {
			r = null_n;
		}
	}

	return r;
}

std::ostream &fdsb::operator<<(std::ostream &os, const Nid &n) {
	switch(n.t) {
	case Nid::Type::special:
		switch(n.s) {
		case Nid::Special::null:
			os << "[null]";
			break;
		case Nid::Special::bool_true:
			os << "[true]";
			break;
		case Nid::Special::bool_false:
			os << "[false]";
			break;
		}
		break;
	case Nid::Type::integer:
		os << '[' << n.i << ']';
		break;
	case Nid::Type::real:
		os << '[' << n.d << ']';
		break;
	default:
		break;
	}
	return os;
}
