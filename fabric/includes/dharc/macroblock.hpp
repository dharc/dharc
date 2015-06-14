/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_MACROBLOCK_HPP_
#define DHARC_FABRIC_MACROBLOCK_HPP_

#include <vector>
#include <cassert>
#include <unordered_map>

#include "dharc/node.hpp"

using dharc::Node;
using std::vector;
using std::unordered_map;

namespace dharc {
namespace fabric {
template<typename T>
class MicroBlock;

class MacroBlockBase {
	public:
	virtual ~MacroBlockBase() {};

	virtual void writeInput(const vector<float> &v) = 0;
	virtual void process(int factor) = 0;
	virtual void pulse(const Node &n) = 0;
	virtual vector<Node> strongestAssociated(float active) = 0;

	virtual size_t harcCount() const = 0;
};



template <typename T>
class MacroBlock : public MacroBlockBase {
	public:
	MacroBlock(const Node &b, size_t w, size_t h);
	~MacroBlock();

	void writeInput(const vector<float> &v);

	void process(int factor);

	inline void pulse(const Node &n) {
		getMicro(n)->pulse(n);
	}

	inline MicroBlock<T> *getMicro(const Node &b) {
		assert(b.macro() == base_.macro());
		assert(b.macroX() < block_w_ && b.macroY() < block_h_);
		return blocks_[(b.macroY()*block_w_) + b.macroX()];
	}

	inline Harc *get(const Node &b) {
		return getMicro(b)->get(b);
	}

	MicroBlock<T> *neighbour(const Node &b, int x, int y);

	size_t harcCount() const;

	void addStrong(const Node &node, const vector<Node> &tvec);
	vector<Node> strongestAssociated(float active);

	typedef T params;

	private:
	Node base_;
	size_t width_;
	size_t height_;
	size_t block_w_;
	size_t block_h_;
	vector<MicroBlock<T>*> blocks_;
	unordered_map<uint64_t, vector<Node>> strong_;
};
};
};

#endif  // DHARC_FABRIC_MACROBLOCK_HPP_

