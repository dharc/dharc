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

#include "dharc/harc.hpp"

using dharc::Fabric;
using dharc::fabric::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;
using dharc::Tail;


atomic<unsigned long long> Fabric::counter__(0);

unordered_map<Tail, Node> Fabric::tails__;
vector<array<Harc, Fabric::HARC_BLOCK_SIZE>*> Fabric::harcs__;

std::atomic<size_t> Fabric::branchcount__(0);
std::atomic<size_t> Fabric::harccount__(0);
std::atomic<size_t> Fabric::activatecount__(0);
std::atomic<size_t> Fabric::followcount__(0);




void Fabric::counterThread() {
	while (true) {
		++counter__;
		std::this_thread::sleep_for(
				std::chrono::milliseconds(counterResolution()));
	}
}



void Fabric::initialise() {
	std::thread t(counterThread);
	t.detach();

}



void Fabric::finalise() {
}



Node Fabric::get(const Tail &key) {
	auto it = tails__.find(key);

	if (it != tails__.end()) {
		return it->second;
	}
	return dharc::null_n;
}


void Fabric::activate(const Node &n, float value) {
	get(n)->activate(value);
	// TODO(knicos): Add to activated list??
}

void Fabric::activate(const Node &first,
						const Node &last,
						const vector<float> &amount) {
	const size_t count = last.value - first.value;
	activatecount__ += count;

	for (size_t i = 0; i < count; ++i) {
		get(Node(first.value + i))->activate(amount[i]);
		// TODO(knicos): Add to activated list??
	}
}



/* Node Fabric::query(const Tail &tail) {
	++querycount__;
	return get(tail).query();
} */

void Fabric::define(const Tail &tail, const Node &head) {
	Node hnode = get(tail);

	if (hnode == dharc::null_n) {
		hnode = makeHarc();
		tails__.insert({tail, hnode});
	}

	++followcount__;

	Harc *harc = get(hnode);
	harc->define(head);
	// TODO(knicos): Activate, add to list ... head as well??
}



Node Fabric::makeHarc() {
	const Node node = Node(harccount__.fetch_add(1));
	const auto x = node.value / HARC_BLOCK_SIZE;

	if (x >= harcs__.size()) {
		harcs__.push_back(new array<Harc, HARC_BLOCK_SIZE>);
	}
	return node;
}



void Fabric::makeHarcs(int count, Node &first, Node &last) {
	first.value = harccount__.fetch_add(count);
	last.value = first.value + count - 1;

	for (int i = 0; i < count; i++) {
		const auto x = (first.value + i) / HARC_BLOCK_SIZE;
		if (x >= harcs__.size()) {
			harcs__.push_back(new array<Harc, HARC_BLOCK_SIZE>);
		}
	}
}



