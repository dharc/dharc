/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/tail.hpp"

#include <algorithm>

dharc::Tail::Tail() {
	// Assume tail construction using insert, so reserve space
	nodes_.reserve(10);
}

dharc::Tail::Tail(size_t n) {
	nodes_.reserve(n);
}

dharc::Tail::Tail(std::initializer_list<dharc::Node> il, bool prefixed) :
	nodes_(il) {
	if (!prefixed) fixup();
}

dharc::Tail::Tail(const dharc::Tail &other) : nodes_(other.nodes_) {}

int dharc::Tail::fixup() {
	std::sort(nodes_.begin(), nodes_.end());
	auto it = std::unique(nodes_.begin(), nodes_.end());
	nodes_.resize(std::distance(nodes_.begin(), it));
	return nodes_.size();
}

void dharc::Tail::insert(const dharc::Node &node) {
	auto it = std::lower_bound(nodes_.begin(), nodes_.end(), node);
	if (it == nodes_.end()) {
		nodes_.push_back(node);
	} else {
		if (*it == node) return;
		nodes_.insert(it, node);
	}
}


std::ostream &dharc::operator<<(std::ostream &os, const dharc::Tail &n) {
	os << '[';
	for (auto i : n.nodes_) {
		os << i << ',';
	}
	//os.seekp(-1, std::ios_base::cur);
	os << ']';
	return os;
}

