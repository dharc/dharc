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
class Tail {
	public:
	Tail();
	explicit Tail(size_t n);
	explicit Tail(std::initializer_list<dharc::Node> il);

	inline bool operator==(const Tail &o) const { return nodes_ == o.nodes_; }

	/**
	 * Fixes an arbitrary vector of nodes to be a valid tail.
	 *     Performs a sort and removes duplicate elements. Not needed if the
	 *     tail is formed correctly either manually or using insert.
	 */
	void fixup();

	/**
	 * Insert a node into a tail vector.
	 *     This function performs an insertion sort that also makes sure there
	 *     are no duplicate nodes in the tail. Using this function to build a
	 *     tail ensures that tailFixup is not required.
	 */
	void insert(const dharc::Node &node);


	auto begin() const { return nodes_.begin(); }
	auto end() const { return nodes_.end(); }
	auto cbegin() const { return nodes_.cbegin(); }
	auto cend() const { return nodes_.cend(); }

	inline auto size() const { return nodes_.size(); }

	inline auto at(size_t index) const { return nodes_.at(index); }

	inline size_t hash() const {
		// FNV-1a Hash of entire tail.
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
		return h;
	}

	private:
	std::vector<dharc::Node> nodes_;
};


namespace fabric {
class Harc;
typedef std::unordered_map<Tail, Harc*> HarcMap;
};  // namespace fabric
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

