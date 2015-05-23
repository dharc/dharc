/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/nid.hpp"

#include <atomic>
#include <ostream>
#include <string>
#include <sstream>

#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"

using dharc::Nid;
using std::string;
using std::stringstream;

std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique() {
	return {Nid::Type::allocated, last_nid++};
}

Nid Nid::from_string(const std::string &str) {
	Nid r = null_n;

	if (str.size() > 0) {
		if (str.at(0) >= '0' && str.at(0) <= '9') {
			if (str.find(':') != string::npos) {
				size_t colon = str.find(':');
				string first = str.substr(0, colon);
				string second = str.substr(colon+1, str.size() - colon);
				r.t = static_cast<Nid::Type>(stoi(first));
				r.i = stoll(second);
			} else if (str.find('.') != string::npos) {
				r.t = Nid::Type::real;
				r.d = stold(str);
			} else {
				r.t = Nid::Type::integer;
				r.i = stoll(str);
			}
		} else if (str.at(0) == '\'') {
			r.t = Nid::Type::character;
			r.c = str.at(1);
		} else if (str.at(0) == '[') {
			string str2 = str.substr(1, str.size() - 2);
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

string Nid::to_string() const {
	stringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream &dharc::operator<<(std::ostream &os, const Nid &n) {
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
	case Nid::Type::character:
		os << "['" << n.c << "']";
		break;
	default:
		os << '[' << static_cast<int>(n.t) << ':' << n.i << ']';
		break;
	}
	return os;
}
