/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_SIGNIFICANCE_HPP_
#define DHARC_FABRIC_SIGNIFICANCE_HPP_

#include "dharc/fabric.hpp"

using dharc::Fabric;

namespace dharc {
namespace fabric {
class Significance {
 public:
	Significance() :
		lastboost_(Fabric::counter()),
		sts_(0.0),
		lts_(0.0) {}



	float significance() const {
		float delta = static_cast<float>(Fabric::counter()) -
						static_cast<float>(lastboost_);
		if (delta == 0) delta = 1.0;
		return 1.0 / delta;
	}



	bool boost(float factor) {
		if (lastboost_ < Fabric::counter()) {
			return true;
		}
		return false;
	}



	void decay(float factor);



	float lastBoost() const {
		float delta = static_cast<float>(Fabric::counter()) -
						static_cast<float>(lastboost_);
		return (delta * Fabric::counterResolution()) / 1000.0f;
	}



 private:
	unsigned long long lastboost_;
	float sts_;
	float lts_;
};
};  // namespace fabric
};  // namespace dharc

#endif  // DHARC_FABRIC_SIGNIFICANCE_HPP_

