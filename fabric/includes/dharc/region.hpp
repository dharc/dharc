/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_REGION_HPP_
#define DHARC_FABRIC_REGION_HPP_

#include <vector>
#include <mutex>
#include <cassert>
#include <cmath>

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
	static constexpr auto kUnitSqrt = (size_t)std::sqrt(USIZE);
	static constexpr auto kUnitCount = UNITSX * UNITSY;
	static constexpr auto kInputSize = USIZE * kUnitCount;
	static constexpr auto kOutputSize = TMAX * kUnitCount;
	static constexpr auto kSpatialLinks = USIZE * USIZE * SMAX;
	static constexpr auto kTemporalLinks = SMAX * TMAX;
	static constexpr auto kDecayRate = 0.5f;
	static constexpr auto kLearnRate = 0.1f;
	static constexpr auto kModChangeRate = 0.9f;
	static constexpr auto kModFactor = 0.5f;
	

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
		float scount[SMAX];
		float slinks[kSpatialLinks];
		uint8_t tlinks[kTemporalLinks];
	};

	void initUnit(size_t ix);
	void processUnit(size_t ix);
	size_t activate(Unit &unit, float *inputs, size_t insize, float *links, float *counts, float *outputs, size_t outsize);
	//void adjust(size_t s, Unit &unit, float *inputs, size_t insize, float *links, float *counts, float *outputs, size_t outsize);
	void activateTemporal(size_t ix);
	void adjustTemporal(size_t);
	void adjustModulation();

	void clearInput();

	inline Unit &getUnit(size_t x, size_t y) {
		return units_[x + (y * UNITSX)];
	}

	inline void boostModulation(int x, int y, float delta) {
		if (x < 0 || x >= (int)UNITSX || y < 0 || y >= (int)UNITSY) return;
		float &mod = getUnit(x,y).modulation;
		mod += (delta < 0.0f) ? mod * delta : (1.0f - mod) * delta;
	}

	Unit units_[kUnitCount];
	
};
};
};

#endif  // DHARC_FABRIC_REGION_HPP_

