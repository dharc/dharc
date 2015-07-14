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
using dharc::fabric::RegionBase;



atomic<unsigned long long> Fabric::counter__(0);
atomic<size_t> Fabric::processed__(0);
vector<RegionBase*> Fabric::regions__;




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
		new Region<50,64,48,10,20>();

	std::thread t(counterThread);
	t.detach();


	std::thread p(processThread);
	p.detach();
	//std::thread p2(processThread);
	//p2.detach();
}



void Fabric::finalise() {
}



vector<int8_t> Fabric::reform2DSigned(RegionID regid, size_t uw, size_t uh) {
	vector<int8_t> res;
	RegionBase *reg = getRegion(regid);
	if (reg == nullptr) return res;

	const auto usx = reg->unitsX();
	const auto usy = reg->unitsY();
	const auto usize = reg->unitSize();

	vector<float> out;
	out.resize(usize * usx * usy);
	reg->reform(out);
	res.resize(usx * uw * usy * uh);

	for (auto i = 0U; i < res.size(); ++i) {
		const auto y = i / (uw * usx);
		const auto x = i % (uw * usx);
		const auto uy = y / uh;
		const auto oy = y % uh;
		const auto ux = x / uw;
		const auto ox = x % uw;
		const auto uix = ux + (uy * usx);
		const auto j = uix * usize + (ox * 2) + (oy * uw * 2);

		res[i] = (int8_t)((out[j] * 128.0f) + (0.0f - (out[j + 1] * 128.0f)));
		//res[i] = (uint8_t)((out[j]) * 255.0f);
	}

	return res;
}



void Fabric::write2DSigned(
		RegionID regid,
		const vector<int8_t> &v,
		size_t uw, size_t uh) {
	RegionBase *reg = getRegion(regid);
	if (reg == nullptr) return;

	const auto usx = reg->unitsX();
	const auto usy = reg->unitsY();
	const auto usize = reg->unitSize();

	assert(usize == 2 * uw * uh);
	assert(v.size() == usx * usy * usize / 2);

	vector<float> input;
	input.resize(usx * usy * usize);

	for (auto i = 0U; i < v.size(); ++i) {
		const auto y = i / (uw * usx);
		const auto x = i % (uw * usx);
		const auto uy = y / uh;
		const auto oy = y % uh;
		const auto ux = x / uw;
		const auto ox = x % uw;
		const auto uix = ux + (uy * usx);
		const auto j = uix * usize + (ox * 2) + (oy * uw * 2);

		if (v[i] > 0) {
			input[j] = (float)v[i] / 128.0f;
			input[j + 1] = 0.0f;
		} else {
			input[j+1] = (float)(0 - v[i]) / 128.0f;
			input[j] = 0.0f;
		}
	}

	reg->write(input);
}



RegionBase *Fabric::getRegion(RegionID regid) {
	if (static_cast<size_t>(regid) >= regions__.size()) {
		return nullptr;
	} else {
		return regions__[static_cast<size_t>(regid)];
	}
}

