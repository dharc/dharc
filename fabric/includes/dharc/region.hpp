/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_REGION_HPP_
#define DHARC_FABRIC_REGION_HPP_

#include <vector>
#include <mutex>
#include <cassert>

#include "dharc/regions.hpp"

using std::vector;
using dharc::RegionID;

namespace dharc {
namespace fabric {

class RegionBase {
	public:
	RegionBase() {}
	virtual ~RegionBase() {}

	virtual size_t unitsX() = 0;
	virtual size_t unitsY() = 0;
	virtual size_t unitSize() = 0;
	virtual size_t outputSize() = 0;

	virtual void write(const vector<float> &v) = 0;

	virtual void reform(vector<float> &v) = 0;

	virtual void process() = 0;

	virtual void activityMap(vector<uint8_t> &v) = 0;

	//virtual const vector<float> &read() = 0;
};

template<
size_t USIZE,
size_t UNITSX,
size_t UNITSY,
size_t SMAX,
size_t TMAX
>
class Region : public RegionBase {
	public:
	static constexpr auto kUnitCount = UNITSX * UNITSY;
	static constexpr auto kInputSize = USIZE * kUnitCount;
	static constexpr auto kOutputSize = TMAX * kUnitCount;
	static constexpr auto kSpatialLinks = USIZE * USIZE * SMAX;
	static constexpr auto kTemporalLinks = SMAX * TMAX;
	static constexpr auto kSpatialDecay = 0.999f;
	static constexpr auto kTemporalDecay = 0.5f;
	static constexpr auto kThresholdScale = 0.00001f;
	static constexpr auto kLearnScale = 1.0f;

	Region();
	~Region();

	size_t unitsX() { return UNITSX; }
	size_t unitsY() { return UNITSY; }
	size_t unitSize() { return USIZE; }
	size_t outputSize() { return TMAX; }

	void write(const vector<float> &v);

	void process();

	void reform(vector<float> &v);

	void activityMap(vector<uint8_t> &v);

	private:
	float inputs_[kInputSize];
	uint8_t outputs_[kOutputSize];	

	struct Unit {
		float modulation;
		float spatial[SMAX];
		float temporal[TMAX];
		float scontrib[SMAX];
		uint8_t slinks[kSpatialLinks];
		uint8_t tlinks[kTemporalLinks];
	};

	void initUnit(size_t ix);
	void processUnit(size_t ix);
	void decaySpatial(size_t ix);
	size_t activateSpatial(size_t ix);
	void adjustSpatial(size_t ix, size_t s, float depol);
	void decayTemporal(size_t ix);
	void activateTemporal(size_t ix);
	void adjustTemporal(size_t);

	void clearInput();	

	Unit units_[kUnitCount];
	
};
};
};

#endif  // DHARC_FABRIC_REGION_HPP_

