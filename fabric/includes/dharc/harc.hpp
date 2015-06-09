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

	/**
	 * Define the Harc as having a constant head node. If there is an
	 * existing non-constant definition, it is removed.
	 * @param d Node the Harc points to.
	 */
	void define(const Node &);

	void activate(float value);

	float significance() const;


	/**
	 * Calculate the significance value between 0.0 and 1.0 of this hyper-arc.
	 * @return Signficance of Harc between 0.0 and 1.0.
	 */
	// inline float significance() const { return sig_.significance(); }


	private:
	Node        head_;
	float       activation_;
	float       delta_;
	float       strength_;
	uint64_t    lastactive_;
	dharc::Lock lock_;

	inline float lastActive() const;

	float decayedActivation() const;
};



std::ostream &operator<<(std::ostream &os, const Harc &h);

};  // namespace fabric
};  // namespace dharc

#endif  // DHARC_HARC_HPP_

