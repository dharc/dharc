/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/tail.hpp"

#include <algorithm>

using dharc::Tail;

namespace {
inline void shift_left(Tail& n, const int bits) {
    //assert(bits <= 32);
    const int ibits = 32 - bits;

    for (int i = 0; i < 7; ++i)
    {
        n.d[i] <<= bits;
        n.d[i] |= n.d[i + 1] >> ibits;
    }
    n.d[7] <<= bits;
}



inline void shift_left_160(Tail& n)
{
    n.d[0] = n.d[5];
    n.d[1] = n.d[6];
    n.d[2] = n.d[7];
    n.d[3] = 0;
    n.d[4] = 0;
    n.d[5] = 0;
    n.d[6] = 0;
    n.d[7] = 0;
}



uint32_t accumulate(Tail& a, const Tail& b) {
    uint64_t c = 0;
    for (int i = 7; i >= 0; --i)
    {
        uint64_t nd = c + a.d[i] + b.d[i];
        a.d[i] = (uint32_t)nd;
        c = nd >> 32;
    }
    return (uint32_t)c;
}
};

bool dharc::Tail::operator==(const Tail &o) const {
	return d[0] == o.d[0] &&
			d[1] == o.d[1] &&
			d[2] == o.d[2] &&
			d[3] == o.d[3] &&
			d[4] == o.d[4] &&
			d[5] == o.d[5] &&
			d[6] == o.d[6] &&
			d[7] == o.d[7];
}

void dharc::Tail::make(std::vector<Node> &tail, Tail &res) {
	std::sort(tail.begin(), tail.end());
	auto it = std::unique(tail.begin(), tail.end());
	tail.resize(std::distance(tail.begin(), it));

	const uint8_t *str = (const uint8_t*)tail.data();

	Tail hash = {
        0xdd268dbcu, 0xaac55036u, 0x2d98c384u, 0xc4e576ccu,
        0xc8b15368u, 0x47b6bbb3u, 0x1023b4c8u, 0xcaee0535u
    };

	size_t len = tail.size() * sizeof(Node);
	for (auto i = 0U; i < len; ++i) {
		hash.d[7] ^= str[i];

		Tail t = hash;
		shift_left(t, 1);
        accumulate(hash, t);
 
        shift_left(t, 4);
        accumulate(hash, t);
 
        shift_left(t, 1);
        accumulate(hash, t);
 
        shift_left(t, 2);
        accumulate(hash, t);
 
        shift_left_160(t);
        accumulate(hash, t);
	}

	res = hash;
}

std::ostream &dharc::operator<<(std::ostream &os, const dharc::Tail &n) {
	/*os << '[';
	for (auto i : n.nodes_) {
		os << i << ',';
	}
	//os.seekp(-1, std::ios_base::cur);
	os << ']';*/
	return os;
}

