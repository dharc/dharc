/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/nid.hpp"

#include <atomic>

#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using fdsb::Nid;

std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique() {
	return {Nid::Type::allocated, last_nid++};
}
