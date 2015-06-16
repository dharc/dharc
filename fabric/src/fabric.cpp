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

#include "dharc/harc.hpp"
#include "dharc/block_types.hpp"

#include "macroblock.cpp"

using dharc::Fabric;
using dharc::fabric::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;
using dharc::Tail;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using dharc::fabric::MacroBlockBase;
using dharc::fabric::MacroBlock;
using dharc::fabric::RawSense;


/*inline bool Fabric::harcCompare(const Node &a, const Node &b) {
	return get(a)->significance() > get(b)->significance();
}


inline bool Fabric::harcMin(const Node &a, const Node &b) {
	if (a == dharc::null_n) return true;
	if (b == dharc::null_n) return false;
	return get(a)->significance() < get(b)->significance();
}*/


atomic<unsigned long long> Fabric::counter__(0);
atomic<size_t> Fabric::branchcount__(0);
atomic<size_t> Fabric::cullcount__(0);
atomic<size_t> Fabric::activatecount__(0);
atomic<size_t> Fabric::followcount__(0);
atomic<size_t> Fabric::processed__(0);

vector<MacroBlockBase*> Fabric::blocks__;



void Fabric::counterThread() {
	while (true) {
		++counter__;

		std::this_thread::sleep_for(
			std::chrono::milliseconds(counterResolution()));
	}
}



void Fabric::processThread() {
	while (true) {
		for (auto i : blocks__) {
			i->process(2);
		}
		//std::this_thread::sleep_for(
		//	std::chrono::milliseconds(10));
		std::this_thread::yield();
	}
}



void Fabric::initialise() {
	std::thread t(counterThread);
	t.detach();


	std::thread p(processThread);
	p.detach();
	//std::thread p2(processThread);
	//p2.detach();
}



void Fabric::finalise() {
}



size_t Fabric::harcCount() {
	size_t res = 0;
	for (auto i : blocks__) {
		res += i->harcCount();
	}
	return res;
}



MacroBlockBase *Fabric::getMacro(const Node &b) {
	assert(b.macro() < blocks__.size());
	return blocks__[b.macro()];
}



void Fabric::createInputBlock(size_t w, size_t h, Node &b) {
	b = Node(blocks__.size(), 0, 0, 0);
	blocks__.push_back(new MacroBlock<RawSense>(b, w, h));
}



void Fabric::writeInputBlock(const Node &b, const vector<float> &v) {
	getMacro(b)->writeInput(v);
}

void Fabric::pulse(const Node &n) {
	getMacro(n)->pulse(n);
}



