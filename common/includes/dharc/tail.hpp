/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_TAIL_HPP_
#define DHARC_TAIL_HPP_

#include <utility>
#include <unordered_map>
#include <vector>

#include "dharc/node.hpp"

namespace dharc {
typedef std::vector<dharc::Node> Tail;
namespace fabric {
struct TailHash {
	inline size_t operator()(const dharc::Tail &x) const {
		return x[0].value*3 + x[1].value;
	}
};
class Harc;
typedef std::unordered_map<Tail, Harc*, TailHash> HarcMap;
};  // namespace fabric
};  // namespace dharc

#endif  // DHARC_TAIL_HPP_

