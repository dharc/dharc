/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_NODE_HPP_
#define DHARC_NODE_HPP_

#include <ostream>
#include <istream>
#include <string>
#include <cstdint>

namespace dharc {

/**
 * Node Identifier, Plain Old Data type.
 *     Represents a concept and point in the dharc hypergraph. Two Nodes
 *     together identify an arc in the hypergraph. Any raw data type can
 *     be represented by a corresponding Node and Nodes can be compared with
 *     each other.
 */
struct Node {
	static constexpr uint64_t HARC_MASK        = 0x00000000FFFFFFFF;
	static constexpr uint64_t MACRO_X_MASK     = 0x0000FF0000000000;
	static constexpr uint64_t MACRO_Y_MASK     = 0x000000FF00000000;
	static constexpr uint64_t MACRO_BLOCK_MASK = 0xFFFF000000000000;
	static constexpr uint64_t MICRO_BLOCK_MASK = 0xFFFFFFFF00000000;

	uint64_t value;

	Node() = default;

	constexpr explicit Node(uint64_t v) : value(v) {}

	constexpr Node(uint64_t macro, uint64_t x, uint64_t y, uint64_t micro) :
		value((macro << 48) | (x << 40) | (y << 32) | micro) {}

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

	inline size_t macro() const { return value >> 48; }
	inline size_t macroX() const { return (value & MACRO_X_MASK) >> 40; }
	inline size_t macroY() const { return (value & MACRO_Y_MASK) >> 32; }
	inline size_t harc() const { return value & HARC_MASK; }
	inline size_t micro() const { return value & MICRO_BLOCK_MASK; }
};

/**
 * Integer NID literals.
 * e.g. Nid x = 1234_n;
 */
constexpr Node operator"" _n(unsigned long long value) {
	return Node(static_cast<uint64_t>(value));
}

/* Special node constants */
constexpr Node null_n = Node(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));


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

};  // namespace dharc

#endif  // DHARC_NODE_HPP_
