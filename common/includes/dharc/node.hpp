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

/**
 * Node Identifier, Plain Old Data type.
 *     Represents a concept and point in the dharc hypergraph. Two Nodes
 *     together identify an arc in the hypergraph. Any raw data type can
 *     be represented by a corresponding Node and Nodes can be compared with
 *     each other.
 */
struct Node {
	uint64_t value;

	Node() = default;

	constexpr explicit Node(uint64_t v) : value(v) {}

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
	return Node(static_cast<uint64_t>(value));
}

/* Special node constants */
constexpr Node null_n = Node(static_cast<uint64_t>(0));


/* Relational operators */
constexpr bool operator==(const Node &a, const Node &b) {
	return a.value == b.value;
}

constexpr bool operator!=(const Node &a, const Node &b) {
	return a.value != b.value;
}

constexpr bool operator<(const Node &a, const Node &b) {
	return a.value < b.value;
}

constexpr bool operator>(const Node &a, const Node &b) {
	return a.value > b.value;
}

constexpr bool operator<=(const Node &a, const Node &b) {
	return a.value <= b.value;
}

constexpr bool operator>=(const Node &a, const Node &b) {
	return a.value >= b.value;
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
