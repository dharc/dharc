/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_MICROBLOCK_HPP_
#define DHARC_FABRIC_MICROBLOCK_HPP_

#include <vector>
#include <mutex>

#include "dharc/fabric.hpp"
#include "dharc/node.hpp"

using dharc::Node;
using std::vector;
using dharc::Fabric;

namespace dharc {
namespace fabric {
/**
 * A small group of hyperarcs that a processed together for locally
 * significant relations between them. Each micro block also searches its
 * immediate neighbouring blocks to make significant connections, although
 * local is given priority.
 */
template <
size_t USIZE,
size_t SMAX,
size_t TMAX
>
class Unit {
	friend dharc::Fabric;

	public:
	Unit(Unit *neighbours[8]);
	~Unit();

	void spatialMatch(float *data, size_t distance

	inline Harc *get(const Node &n) {
		assert( n.harc() < params::MAX_BLOCK_HARCS );
		return (n.micro() == blockid_) ? &(harcs_[n.harc()]) : macro_->get(n);
	}

	bool allocate(size_t count, Node &first, Node &last);
	bool allocate(Node &node);

	void pulse(const Node &n);

	void constant(const Node &b, float amount);

	void process(int factor);
	void garbage();

	inline void lock() { lock_.lock(); }
	inline void unlock() { lock_.unlock(); }

	size_t harcCount() const { return harc_count_; }

	inline bool harcCompare(const Node &a, const Node &b);

	private:

	static_assert( params::MAX_BLOCK_HARCS < Node::MICRO_BLOCK_MASK,
					"To many harcs for single block" );

	struct HarcCompare {
		MicroBlock<T> *block;
		inline bool operator()(const Node &a, const Node &b) {
			return block->harcCompare(a,b);
		}
	};

	uint64_t blockid_;
	MacroBlock<T> *macro_;
	std::mutex lock_;
	set<Node, HarcCompare> sig_;
	array<Harc, params::MAX_BLOCK_HARCS> harcs_;
	size_t harc_count_;
	vector<int> freed_;
	unordered_map<Tail, Node> tails_;
	size_t lastgarbage_;

	bool get(const Tail &key, Node &hnode);
	bool query(const Tail &tail, const vector<Node> &tvec, float sig);
	void addToQueue(const Node &node, Harc *harc);
	void add(const Tail &tail, float sig);
	inline size_t distance(const Node &a, const Node &b);
	Node sigNeighbour(const Node *signodes, size_t count, float minsig);
};
};
};


#endif  // DHARC_FABRIC_MICROBLOCK_HPP_

