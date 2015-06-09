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

using std::vector;
using std::array;
using std::set;
using std::unordered_map;
using std::chrono::time_point;
using std::size_t;
using dharc::fabric::Harc;
using dharc::Tail;
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

	public:
	Fabric() = delete;

	static void initialise();
	static void finalise();



	/**
	 * Query the head node of a hyperarc.
	 *     Given the tail nodes of a hyperarc, lookup the hyperarc and ask for
	 *     its head node. This may involve the evaluation of the hyperarcs
	 *     definition if it is out-of-date. If the requested hyperarc does not
	 *     exist then it is created and the head node null is returned. A query
	 *     will also activate a hyperarch and modify is significance and
	 *     activation status.
	 * @param tail Pair of tail nodes to identify the hyperarc.
	 * @return Head node of hyperarch.
	 */
	// static Node follow(const Tail &tail);

	//static Node query(const Node &a, const Node &b) {
	//	return query(Tail{a, b});
	// }



	/**
	 * Give a harc a constant head node.
	 *     Removes any existing definition and sets the head of the harc to
	 *     the given node. This will cause an activation of the harc and will
	 *     log this harc as having changed if the harcs log flag is set. If the
	 *     harc does not exist, it is created.
	 * @see define
	 * @param tail A set of tail nodes to identify the harcs.
	 * @param head The new head node for the harc.
	 */
	static void define(const Tail &tail, const Node &head);



	static void activate(const Node &n, float value);

	static void activate(const Node &first,
						const Node &last,
						const vector<float> &amount);



	/**
	 * Make a single harc with no tail.
	 * @return Node to identify the harc.
	 */
	static Node makeHarc();



	/**
	 * Make block of harcs that have no tails or heads.
	 * @param count Number of harcs to create.
	 * @param first Filled with first harc node.
	 * @param last Filled with last (first+count).
	 */
	static void makeHarcs(int count, Node &first, Node &last);


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
	static size_t harcCount()        { return harccount__; }


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
	static constexpr size_t HARC_BLOCK_SIZE = 4096;
	static constexpr size_t MAX_UNPROCESSED = 20;
	static constexpr size_t SIGNIFICANT_QUEUE_SIZE = 20;
	static constexpr float SIG_THRESHOLD = 0.5f;

	static unordered_map<Tail, Node> tails__;
	static vector<array<Harc, HARC_BLOCK_SIZE>*> harcs__;

	static set<Node, bool(*)(const Node &, const Node &)> unproc__;
	static std::mutex unproc_lock__;

	static array<Node, SIGNIFICANT_QUEUE_SIZE> sigharcs__;
	static std::mutex sigharcs_lock__;

	static std::atomic<size_t> branchcount__;
	static std::atomic<size_t> harccount__;
	static std::atomic<size_t> activatecount__;
	static std::atomic<size_t> followcount__;
	static std::atomic<size_t> processed__;

	static std::atomic<unsigned long long> counter__;

	static Node get(const Tail &key);

	inline static Harc *get(const Node &node) {
		const auto x = node.value / HARC_BLOCK_SIZE;
		const auto y = node.value % HARC_BLOCK_SIZE;
		assert(x < harcs__.size());
		return &harcs__[x]->at(y);
	}

	static void counterThread();
	static void processThread();

	static void addToQueue(const Node &node, Harc *harc);

	inline static bool harcCompare(const Node &a, const Node &b);
	inline static bool harcMin(const Node &a, const Node &b);
};
};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

