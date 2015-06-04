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

dharc::Tail::Tail(std::initializer_list<dharc::Node> il) :
	nodes_(il) {
	fixup();
}

void dharc::Tail::fixup() {
	std::sort(nodes_.begin(), nodes_.end());
	auto it = std::unique(nodes_.begin(), nodes_.end());
	nodes_.resize(std::distance(nodes_.begin(),it));
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

