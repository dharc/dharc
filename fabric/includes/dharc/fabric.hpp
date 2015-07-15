/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_HPP_
#define DHARC_FABRIC_HPP_

#include <vector>
#include <chrono>
#include <utility>
#include <atomic>
#include <cassert>
#include <mutex>

#include "dharc/region.hpp"

using std::vector;
using std::chrono::time_point;
using std::size_t;
using dharc::fabric::RegionBase;
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
	public:
	Fabric() = delete;

	static void initialise();
	static void finalise();

	static void write2D(RegionID regid, const vector<uint8_t> &v, size_t uw, size_t uh);

	static vector<uint8_t> reform2D(RegionID regid, size_t uw, size_t uh);

	static RegionBase *getRegion(RegionID regid);

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
	static std::atomic<size_t> processed__;

	static std::atomic<unsigned long long> counter__;

	static vector<RegionBase*> regions__;
	//static vector<vector<float>> region_inputs__;

	static void counterThread();
	static void processThread();
};
};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

