/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PACKER_HPP_
#define DHARC_PACKER_HPP_

#include <iostream>
#include <vector>
#include <list>

#include "dharc/node.hpp"
#include "dharc/tail.hpp"

namespace dharc {
namespace rpc {

/**
 * Default RPC packer for anything supporting both stream operators.
 */
template<typename T>
struct Packer {
	static void pack(std::ostream &os, const T &first) {
		os << first;
	}
	static T unpack(std::istream &is) {
		T res;
		is >> res;
		return res;
	}
};

template <>
struct Packer<dharc::Node> {
	static void pack(std::ostream &os, const dharc::Node &n) {
		os << n.value;
	}
	static dharc::Node unpack(std::istream &is) {
		dharc::Node res;
		is >> res.value;
		return res;
	}
};

/**
 * RPC packer for vectors.
 */
template<>
template<typename R>
struct Packer<std::vector<R>> {
	static void pack(std::ostream &os, const std::vector<R> &vec) {
		auto x = vec.size();
		os << '[';
		for (auto i : vec) {
			--x;
			Packer<R>::pack(os, i);
			if (x != 0) os << ',';
		}
		os << ']';
	}
	static std::vector<R> unpack(std::istream &is) {
		std::vector<R> res;
		if (is.get() != '[') return res;
		while (is.peek() != ']') {
			res.push_back(Packer<R>::unpack(is));
			if (is.peek() == ',') is.ignore();
		}
		is.ignore();  // Remove trailing ']'
		return res;
	}
};

/**
 * RPC packer for Tails.
 */
template<>
struct Packer<dharc::Tail> {
	static void pack(std::ostream &os, const dharc::Tail &tail) {
		auto x = tail.size();
		os << '[';
		for (auto i : tail) {
			--x;
			Packer<Node>::pack(os, i);
			if (x != 0) os << ',';
		}
		os << ']';
	}
	static dharc::Tail unpack(std::istream &is) {
		dharc::Tail res;
		if (is.get() != '[') return res;
		while (is.peek() != ']') {
			res.insertRaw(Packer<Node>::unpack(is));
			if (is.peek() == ',') is.ignore();
		}
		is.ignore();  // Remove trailing ']'
		return res;
	}
};

/**
 * RPC packer for lists.
 */
template<>
template<typename R>
struct Packer<std::list<R>> {
	static void pack(std::ostream &os, const std::list<R> &lst) {
		auto x = lst.size();
		os << '[';
		for (auto i : lst) {
			--x;
			Packer<R>::pack(os, i);
			if (x != 0) os << ',';
		}
		os << ']';
	}
	static std::list<R> unpack(std::istream &is) {
		std::list<R> res;
		if (is.get() != '[') return res;
		while (is.peek() != ']') {
			res.push_back(Packer<R>::unpack(is));
			if (is.peek() == ',') is.ignore();
		}
		is.ignore();  // Remove trailing ']'
		return res;
	}
};

};  // namespace rpc
};  // namespace dharc

#endif  // DHARC_PACKER_HPP_

