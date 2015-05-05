/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_FABRIC_H_
#define FDSB_FABRIC_H_

#include <vector>

#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

namespace fdsb {

inline Harc &get(const Nid &a, const Nid &b) { return Harc::get(a, b); }
inline Nid path(const fdsb::Path &p) {
	return Harc::path(p, nullptr);
}

};

#endif /* _FDSB_FABRIC_H_ */
