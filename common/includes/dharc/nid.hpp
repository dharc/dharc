/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_NID_H_
#define DHARC_NID_H_

#include <ostream>
#include <istream>
#include <string>

#include "dharc/rpc_packer.hpp"

namespace dharc {

class Harc;

/**
 * Node Identifier Plain Old Data type.
 */
struct Nid {
	enum struct Type : unsigned char {
		special,
		integer,
		real,
		character,
		allocated,
	};

	enum struct Special {
		null,
		bool_true,
		bool_false,
	};

	Type t;
	union {
	unsigned long long i;
	Special s;
	long double d;
	char c;
	};

	Nid operator[](const Nid &);

	std::string to_string() const;

	/**
	 * Generate a new unique node id.
	 */
	static Nid unique();

	static Nid from_string(const std::string &str);

	constexpr static Nid from_int(unsigned long long v) {
		return {Nid::Type::integer, {.i = v}};
	}

	constexpr static Nid from_double(long double v) {
		return Nid{Nid::Type::real, { .d = v }};
	}
};

/**
 * Integer NID literals.
 * e.g. Nid x = 1234_nid;
 */
constexpr Nid operator"" _n(unsigned long long v) {
	return Nid::from_int(v);
}

/**
 * Real NID literals.
 * e.g. Nid x = 12.34_nid;
 */
constexpr Nid operator"" _n(long double v) {
	return Nid::from_double(v);
}

/**
 * Character NID literals.
 * e.g. Nid x = 'a'_nid;
 */
constexpr Nid operator"" _n(char v) {
	return Nid{Nid::Type::character, { .c = v }};
}

constexpr Nid null_n = {Nid::Type::special, { .s = Nid::Special::null }};
constexpr Nid true_n = {Nid::Type::special, { .s = Nid::Special::bool_true }};
constexpr Nid false_n = {Nid::Type::special, { .s = Nid::Special::bool_false }};

/**
 * NID equality comparison.
 */
constexpr bool operator==(const Nid &a, const Nid &b) {
	return a.t == b.t && a.i == b.i;
}

/**
 * NID inequality comparison.
 */
constexpr bool operator!=(const Nid &a, const Nid &b) {
	return a.t != b.t || a.i != b.i;
}

constexpr bool operator<(const Nid &a, const Nid &b) {
	return a.t < b.t || (a.t == b.t && a.i < b.i);
}

constexpr bool operator>(const Nid &a, const Nid &b) {
	return a.t > b.t || (a.t == b.t && a.i > b.i);
}

constexpr bool operator<=(const Nid &a, const Nid &b) {
	return a.t <= b.t || (a.t == b.t && a.i <= b.i);
}

constexpr bool operator>=(const Nid &a, const Nid &b) {
	return a.t >= b.t || (a.t == b.t && a.i >= b.i);
}

std::ostream &operator<<(std::ostream &os, const Nid &n);

namespace rpc {
template <>
struct Packer<Nid> {
	static void pack(std::ostream &os, const Nid &n);
	static Nid unpack(std::istream &is);
};
};  // namespace rpc
};  // namespace dharc

#endif
