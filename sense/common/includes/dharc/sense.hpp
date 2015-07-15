/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_SENSE_HPP_
#define DHARC_SENSE_HPP_

#include "dharc/rpc.hpp"
#include "dharc/regions.hpp"

using dharc::RegionID;

namespace dharc {

class Sense : public dharc::Rpc {
	public:
	Sense(const char *addr, int port);
	~Sense();

	void write2D(
		RegionID regid,
		const vector<uint8_t> &values,
		size_t uw, size_t uh);

	vector<uint8_t> reform2D(RegionID regid, size_t uw, size_t uh);
};

};

#endif  // DHARC_SENSE_HPP_

