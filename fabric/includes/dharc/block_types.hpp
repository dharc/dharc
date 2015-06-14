/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_BLOCK_TYPES_HPP_
#define DHARC_FABRIC_BLOCK_TYPES_HPP_

namespace dharc {
namespace fabric {
struct RawSense {
	static constexpr size_t BLOCK_WIDTH = 10;
	static constexpr size_t MAX_BLOCK_HARCS =   BLOCK_WIDTH *
												BLOCK_WIDTH *
												20;
	static constexpr size_t MAX_SIGNIFICANT = 100;
	static constexpr float SIG_THRESHOLD = 0.03;
	static constexpr size_t MAX_TAIL = 10;
	static constexpr size_t MIN_TAIL = 3;
};
};
};

#endif  // DHARC_FABRIC_BLOCK_TYPES_HPP_

