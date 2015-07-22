/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/fabric.hpp"

#include <future>
#include <thread>
#include <vector>
#include <utility>
#include <list>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <map>
#include <functional>
#include <deque>
#include <iostream>
#include <mutex>

#include "dharc/region.hpp"

#include "region.cpp"

using dharc::Fabric;
using std::vector;
using std::list;
using std::atomic;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using dharc::fabric::Region;



atomic<unsigned long long> Fabric::counter__(0);
atomic<size_t> Fabric::processed__(0);
vector<Region*> Fabric::regions__;




void Fabric::counterThread() {
	while (true) {
		++counter__;

		std::this_thread::sleep_for(
			std::chrono::milliseconds(counterResolution()));
	}
}



void Fabric::processThread() {
	while (true) {
		for (auto i : regions__) {
			i->process();
		}
		//std::this_thread::sleep_for(
			//std::chrono::milliseconds(10));
		std::this_thread::yield();
	}
}



void Fabric::initialise() {
	regions__.resize(1);

	regions__[static_cast<size_t>(RegionID::SENSE_CAMERA_0_LUMINANCE)] =
		new Region(320, 240, 64, 48);

	std::thread t(counterThread);
	t.detach();


	std::thread p(processThread);
	p.detach();
	//std::thread p2(processThread);
	//p2.detach();
}



void Fabric::finalise() {
}



vector<uint8_t> Fabric::reform2D(RegionID regid, size_t uw, size_t uh) {
	vector<uint8_t> out;
	Region *reg = getRegion(regid);
	if (reg == nullptr) return out;

	reg->reform(out);
	return out;
}



void Fabric::write2D(
		RegionID regid,
		const vector<uint8_t> &v) {
	Region *reg = getRegion(regid);
	if (reg == nullptr) return;

	reg->write(v);
}



Region *Fabric::getRegion(RegionID regid) {
	if (static_cast<size_t>(regid) >= regions__.size()) {
		return nullptr;
	} else {
		return regions__[static_cast<size_t>(regid)];
	}
}

