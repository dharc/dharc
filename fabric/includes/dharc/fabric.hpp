/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_HPP_
#define DHARC_FABRIC_HPP_

#include <vector>
#include <chrono>
#include <utility>
#include <atomic>
#include <unordered_map>
#include <array>
#include <cassert>
#include <set>
#include <mutex>

#include "dharc/node.hpp"
#include "dharc/harc.hpp"
#include "dharc/tail.hpp"
#include "dharc/macroblock.hpp"

using std::vector;
using std::array;
using std::set;
using std::unordered_map;
using std::chrono::time_point;
using std::size_t;
using dharc::fabric::Harc;
using dharc::Tail;
using dharc::fabric::MacroBlockBase;
// using dharc::LIFOBuffer;

namespace dharc {
/**
 * The Dharc Fabric: a dynamic hypergraph.
 *     Stores all the nodes and hyperarcs relating the nodes together. Each
 *     hyperarc (harc) has metrics about its significance and activation to
 *     improve pattern searching and suggestion. A harc can also be given a
 *     definition that frames it in-terms of other nodes and harcs. Low-level
 *     search and access functions are provided, usually to significance sorted
 *     data, that can then be manipulated by higher-level pattern matching and
 *     search algorithms. This class hides any storage concerns and is
 *     threadsafe. The class is entirely static, only one fabric is possible
 *     per instance of the fabric server.
 */
class Fabric {
	friend dharc::fabric::Harc;
	// friend dharc::fabric::MicroBlock;

	public:
	Fabric() = delete;

	static void initialise();
	static void finalise();


	/**
	 * Send an activation pulse to a Harc at global level.
	 */
	static void pulse(const Node &n);


	/**
	 * Make block of harcs that have no tails or heads.
	 * @param count Number of harcs to create.
	 * @param first Filled with first harc node.
	 * @param last Filled with last (first+count).
	 */
	static void createInputBlock(size_t w, size_t h, Node &base);

	static void writeInputBlock(const Node &b, const vector<float> &v);

	static void createOutputBlock(size_t width, size_t height, Node &base);

	static void readOutputBlock(const Node &b, vector<float> &v);


	/**
	 * Statistic: Number of hyperarcs.
	 * @return Total hyperarcs in the fabric
	 */
	static size_t branchCount()        { return branchcount__; }



	/**
	 * Statistic: Number of nodes.
	 *     Some of these nodes may not be involved in a hyperarc.
	 * @return Total number of allocated nodes.
	 */
	static size_t harcCount();

	static size_t followCount()        { return followcount__; }


	/**
	 * Statistic: Approximate number of queries per second.
	 */
	static float  followsPerSecond() {
		return (static_cast<float>(followcount__) /
				static_cast<float>(counter__)) *
				static_cast<float>(counterResolution());
	}



	/**
	 * Statistic: Approximate number of hyperarc modifications per second.
	 */
	static float  activationsPerSecond() {
		return (static_cast<float>(activatecount__) /
				static_cast<float>(counter__)) *
				static_cast<float>(counterResolution());
	}

	/**
	 * Statistic: Approximate number of hyperarc modifications per second.
	 */
	static float  processedPerSecond() {
		return (static_cast<float>(processed__) /
				static_cast<float>(counter__)) *
				static_cast<float>(counterResolution());
	}



	/**
	 * Number of ticks since program start. Used to record when a relation
	 * was last accessed or changed.
	 */
	inline static unsigned long long counter() { return counter__; }



	/**
	 * Number of milliseconds per tick.
	 */
	constexpr static unsigned long long counterResolution() { return 20; }



	private:
	static std::atomic<size_t> branchcount__;
	static std::atomic<size_t> cullcount__;
	static std::atomic<size_t> activatecount__;
	static std::atomic<size_t> followcount__;
	static std::atomic<size_t> processed__;

	static std::atomic<unsigned long long> counter__;

	static vector<MacroBlockBase*> blocks__;

	static void counterThread();
	static void processThread();

	static MacroBlockBase *getMacro(const Node &b);
	/*inline MicroBlock *getMicro(const Node &b) {
		return getMacro(b)->getMicro(b);
	}
	inline Harc *getHarc(const Node &h) {
		return getMacro(h)->getMicro(h)->getHarc(h);
	}*/
};
};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

