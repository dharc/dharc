/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_PACKER_H_
#define DHARC_PACKER_H_

#include <vector>

namespace dharc {
namespace rpc {
template<typename T>
struct Packer {
	static void pack(std::ostream &os, T first) {
		os << first;
	}
	static T unpack(std::istream &is) {
		T res;
		is >> res;
		return res;
	}
};

template<>
template<typename R>
struct Packer<std::vector<R>> {
	static void pack(std::ostream &os, const std::vector<R> &vec) {
		os << '[';
		for (auto i : vec) {
			Packer<R>::pack(os, i);
			os << ',';
		}
		os << ']';
	}
	static std::vector<R> unpack(std::istream &is) {
		std::vector<R> res;
		is.get();  // '['
		res.push_back(Packer<R>::unpack(is));
		// ...
		return res;
	}
};
};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_PACKER_H_ */

