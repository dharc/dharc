/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/nid.hpp"

#include <atomic>
#include <ostream>

#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using fdsb::Nid;

std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique() {
	return {Nid::Type::allocated, last_nid++};
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
