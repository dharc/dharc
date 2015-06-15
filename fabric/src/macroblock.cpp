/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/macroblock.hpp"
#include "dharc/microblock.hpp"

#include "microblock.cpp"

using dharc::fabric::MacroBlock;
using dharc::fabric::MicroBlock;



template<typename T>
MacroBlock<T>::MacroBlock(const Node &b, size_t w, size_t h) :
	base_(b), width_(w), height_(h) {
	block_w_ = width_ / params::BLOCK_WIDTH;
	block_h_ = height_ / params::BLOCK_WIDTH;

	size_t blockcount = block_w_ * block_h_;

	for (auto i = 0U; i < blockcount; ++i) {
		Node bid(base_.macro(),
					i % block_w_,
					i / block_w_, 0);
		blocks_.push_back(new MicroBlock<T>(this, bid.value));
	}
}



template<typename T>
MacroBlock<T>::~MacroBlock() {
	for (auto i : blocks_) {
		delete i;
	}
}



template<typename T>
void MacroBlock<T>::writeInput(const vector<float> &v) {
	auto bix = 0U;
	constexpr auto BW2 = params::BLOCK_WIDTH * params::BLOCK_WIDTH;
	const auto LINE = block_w_ * params::BLOCK_WIDTH;

	assert(v.size() == block_w_ * block_h_ * BW2);

	for (auto y = 0U; y < block_h_; ++y) {
		for (auto x = 0U; x < block_w_; ++x) {
			Node n(base_.macro(), x, y, 0U);
			MicroBlock<T> *block = blocks_[bix++];
			block->lock();

			const auto ox = x * params::BLOCK_WIDTH;
			const auto oy = y * params::BLOCK_WIDTH;
			const auto vix = (oy * LINE) + ox;

			for (auto i = 0U; i < BW2; ++i) {
				const auto vx = i % params::BLOCK_WIDTH;
				const auto vy = i / params::BLOCK_WIDTH;
				const auto vix2 = vix + vx + (vy * LINE);
				block->constant(n, v[vix2]);
				++n.value;
			}
			block->unlock();
		}
	}
}



template<typename T>
void MacroBlock<T>::process(int factor) {
	for (auto i : blocks_) {
		i->process(factor);
	}
}



template<typename T>
MicroBlock<T> *MacroBlock<T>::neighbour(const Node &b, int x, int y) {
	return nullptr;
}



template<typename T>
size_t MacroBlock<T>::harcCount() const {
	size_t res = 0;
	for (auto i : blocks_) {
		res += i->harcCount();
	}
	return res;
}



template<typename T>
void MacroBlock<T>::addStrong(const Node &node, const vector<Node> &tvec) {
	strong_lock_.lock();
	if (strong_.find(node.value) == strong_.end()) {
		strong_.insert({node.value, tvec});
	}
	strong_lock_.unlock();
}



template<typename T>
vector<Node> MacroBlock<T>::strongestAssociated(float active) {
	vector<Node> res;

	strong_lock_.lock();
	auto i = strong_.begin();
	while (i != strong_.end()) {
		if (get(Node((*i).first))->lastActive() < active) {
			for (auto j : (*i).second) {
				res.push_back(j);
			}
			++i;
		} else {
			i = strong_.erase(i);
		}
	}
	strong_lock_.unlock();

	return res;
}

