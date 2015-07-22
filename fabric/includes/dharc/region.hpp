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
class Region {
	public:
	static constexpr auto kSuppressionRate = 0.5;
	static constexpr auto kLearnRate = 0.1f;
	static constexpr auto kContrastMax = 10.0f;

	Region(size_t width, size_t height, size_t unitsx, size_t unitsy);
	~Region();

	void write(const vector<uint8_t> &v);

	void process();

	void reform(vector<uint8_t> &v);

	private:
	const size_t unitsx_;
	const size_t unitsy_;
	const size_t width_;
	const size_t height_;
	const size_t uwidth_;
	const size_t uheight_;
	const size_t outsize_;

	struct Link {
		float strength;
		float depol;
	};

	struct Unit {
		float modulation;
		vector<float> inputs;
		vector<float> outputs;
		vector<float> counts;
		vector<Link> links;
	};

	void makeInputLayer();
	void initUnit(Unit &unit, size_t iwidth, size_t iheight);
	void processLayer(size_t layer);
	void processUnit(Unit &unit);

	vector<vector<vector<Unit>>> units_;
	
};
};
};

#endif  // DHARC_FABRIC_REGION_HPP_

