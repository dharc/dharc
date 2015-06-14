/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_TAIL_HPP_
#define DHARC_TAIL_HPP_

#include <unordered_map>
#include <vector>
#include <functional>

#include "dharc/node.hpp"

namespace dharc {
struct Tail {
	uint32_t d[8];

	bool operator==(const Tail &o) const;

	inline size_t hash() const {
		/*// FNV-1a Hash of entire tail.
		// NOTE(knicos): Assumes 64-bit little-endian machine!
		size_t h = 14695981039346656037U;
		auto data = (const unsigned char*)nodes_.data();
		auto end = data + (nodes_.size() * sizeof(dharc::Node));
		while (data != end) {
			// Unroll 4 times for first 32 bits
			h ^= *(data++);
			h *= 1099511628211;
			h ^= *(data++);
			h *= 1099511628211;
			h ^= *(data++);
			h *= 1099511628211;
			h ^= *(data++);
			h *= 1099511628211;
			// Now skip upper 32-bits that will often be empty
			data += 4;
		}
		return h;*/
		return d[7];
	}

	static void make(std::vector<Node> &t, Tail &res);
};

/* Stream Operators */
std::ostream &operator<<(std::ostream &os, const Tail &n);


};  // namespace dharc

namespace std {
template<>
struct hash<dharc::Tail> {
	inline size_t operator()(const dharc::Tail &x) const {
		return x.hash();
	}
};
};  // namespace std

#endif  // DHARC_TAIL_HPP_

