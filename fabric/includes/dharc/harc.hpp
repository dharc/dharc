/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_HARC_HPP_
#define DHARC_HARC_HPP_

#include "dharc/node.hpp"
#include "dharc/lock.hpp"

using dharc::Node;

namespace dharc {
namespace fabric {

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc {
	public:
	Harc();

	bool query(const Node &node);

	void activateConstant(float value);
	void pulse(float s);

	float significance() const;

	inline float strength() const { return strength_; }

	bool isAvailable() const { return lastactive_ == 0; }
	void notAvailable() { lastactive_ = 1; }

	bool isWeak() const {
		return ((lastActive() > 200.0f) && (strength_ <= 0.1f));
	}

	void reset();

	float lastActive() const;

	/**
	 * Calculate the significance value between 0.0 and 1.0 of this hyper-arc.
	 * @return Signficance of Harc between 0.0 and 1.0.
	 */
	// inline float significance() const { return sig_.significance(); }


	private:
	Node        head_;
	float       delta_;
	union {
	float       activation_;
	float       strength_;
	};
	uint64_t    lastactive_;
	dharc::Lock lock_;

	float decayedActivation() const;
};



std::ostream &operator<<(std::ostream &os, const Harc &h);

};  // namespace fabric
};  // namespace dharc

#endif  // DHARC_HARC_HPP_

