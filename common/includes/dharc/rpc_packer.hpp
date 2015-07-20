/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PACKER_HPP_
#define DHARC_PACKER_HPP_

#include <iostream>
#include <vector>
#include <list>
#include <cassert>
#include <zlib.h>

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
		os.write((const char*)&first, sizeof(T));
	}
	static T unpack(std::istream &is) {
		T res;
		is.read((char*)&res, sizeof(T));
		return res;
	}
};

template <>
struct Packer<dharc::Node> {
	static void pack(std::ostream &os, const dharc::Node &n) {
		os.write((const char*)&n.value, sizeof(uint64_t));
	}
	static dharc::Node unpack(std::istream &is) {
		dharc::Node res;
		is.read((char*)&res.value, sizeof(uint64_t));
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
		long unsigned int x = vec.size();
		os.write((const char *)&x, sizeof(long unsigned int));
		//x = compressBound(x * sizeof(R));
		//unsigned char *buffer = new unsigned char[x];
		//compress(buffer, &x, vec.data(), vec.size() * sizeof(R));
		//os.write((const char *)&x, sizeof(long unsigned int));
		//os.write((const char *)buffer, x);
		os.write((const char *)vec.data(), x * sizeof(R));
		//delete [] buffer;
	}
	static std::vector<R> unpack(std::istream &is) {
		std::vector<R> res;
		long unsigned int x;
		is.read((char *)&x, sizeof(long unsigned int));
		res.resize(x);
		//x *= sizeof(R);
		//long unsigned int ux;
		//is.read((char *)&ux, sizeof(long unsigned int));
		//unsigned char *buffer = new unsigned char[ux];
		//is.read((char *)buffer, ux);
		is.read((char *)res.data(), x * sizeof(R));
		//uncompress(res.data(), &x, buffer, ux);
		//delete [] buffer;
		return res;
	}
};



/**
 * RPC packer for Tails.
 */
/*template<>
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
};*/

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

