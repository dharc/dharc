/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_NODE_HPP_
#define DHARC_NODE_HPP_

#include <ostream>
#include <istream>
#include <string>
#include <cstdint>

#include "dharc/rpc_packer.hpp"

namespace dharc {

static_assert(sizeof(double) <= 64, "Double is too big");

/* Constants for special node types */
enum {
	kNullNode = 0,
	kTrueNode = 1,
	kFalseNode = 2
};

/**
 * Node Identifier, Plain Old Data type.
 *     Represents a concept and point in the dharc hypergraph. Two Nodes
 *     together identify an arc in the hypergraph. Any raw data type can
 *     be represented by a corresponding Node and Nodes can be compared with
 *     each other.
 */
struct Node {
	enum struct Type : uint8_t {
		special,
		integer,
		real,
		character,
		constant,
		allocated,
	};

	Type t;
	union {
	uint64_t ui;
	int64_t i;
	double d;
	char c;
	};

	/* Type casting to a Node */
	Node() = default;
	constexpr Node(Type type, int64_t value) : t(type), i(value) {}
	explicit constexpr Node(int value) : t(Type::integer), i(value) {}
	explicit constexpr Node(float value) : t(Type::real), d(value) {}
	explicit constexpr Node(double value) : t(Type::real), d(value) {}
	explicit constexpr Node(char value) : t(Type::character), i(value) {}
	explicit constexpr Node(bool value)
		: t(Type::special), i((value) ? kTrueNode : kFalseNode) {}

	/**
	 * Generate a Node from a string.
	 *     The string must be a valid format or the null Node is returned.
	 *     Integers (Base 10), floating point numbers, characters expressed as
	 *     'a', the keywords "true", "false and "null" are all valid. Raw Node
	 *     strings of the form "[<type>:<number>]" are also accepted.
	 *
	 * @param str Valid node string.
	 * @return null Node or Node form the string.
	 */
	explicit Node(const std::string &str);

	/** Same as for Node(const std::string &str); */
	explicit Node(const char *str);

	/* Type casting from a Node */
	explicit operator int() const;
	explicit operator float() const;
	explicit operator double() const;
	explicit operator char() const;
	explicit operator bool() const;

	/**
	 * Convert the Node to a string object.
	 *     Special types are converted to corresponding words and numeric types
	 *     are converted to standard base 10 string form. Other kinds of Node
	 *     are shown as <type>:<number>. Calling fromString on the returned
	 *     string is guaranteed to reproduce the same Node.
	 *
	 * @return String representation of the node.
	 */
	explicit operator std::string() const;
};

/**
 * Integer NID literals.
 * e.g. Nid x = 1234_n;
 */
constexpr Node operator"" _n(unsigned long long value) {
	return Node(static_cast<int>(value));
}

/**
 * Real NID literals.
 * e.g. Nid x = 12.34_n;
 */
constexpr Node operator"" _n(long double value) {
	return Node(static_cast<double>(value));
}

/**
 * Character NID literals.
 * e.g. Nid x = 'a'_n;
 */
constexpr Node operator"" _n(char value) {
	return Node(Node::Type::character, value);
}

/* Special node constants */
constexpr Node null_n = Node(Node::Type::special, kNullNode);
constexpr Node true_n = Node(Node::Type::special, kTrueNode);
constexpr Node false_n = Node(Node::Type::special, kFalseNode);

/* Relational operators */
constexpr bool operator==(const Node &a, const Node &b) {
	return a.t == b.t && a.i == b.i;
}

constexpr bool operator!=(const Node &a, const Node &b) {
	return a.t != b.t || a.i != b.i;
}

constexpr bool operator<(const Node &a, const Node &b) {
	return a.t < b.t || (a.t == b.t && a.i < b.i);
}

constexpr bool operator>(const Node &a, const Node &b) {
	return a.t > b.t || (a.t == b.t && a.i > b.i);
}

constexpr bool operator<=(const Node &a, const Node &b) {
	return a.t <= b.t || (a.t == b.t && a.i <= b.i);
}

constexpr bool operator>=(const Node &a, const Node &b) {
	return a.t >= b.t || (a.t == b.t && a.i >= b.i);
}

/* Stream Operators */
std::ostream &operator<<(std::ostream &os, const Node &n);
std::istream &operator>>(std::istream &is, Node &n);

/* Pack and unpack for remote procedure call */
namespace rpc {
template <>
struct Packer<Node> {
	static void pack(std::ostream &os, const Node &n);
	static Node unpack(std::istream &is);
};
};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_NODE_HPP_
