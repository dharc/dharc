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


inline bool Fabric::harcCompare(const Node &a, const Node &b) {
	return get(a)->significance() > get(b)->significance();
}


inline bool Fabric::harcMin(const Node &a, const Node &b) {
	if (a == dharc::null_n) return true;
	if (b == dharc::null_n) return false;
	return get(a)->significance() < get(b)->significance();
}


atomic<unsigned long long> Fabric::counter__(0);

unordered_map<Tail, Node> Fabric::tails__;
vector<array<Harc, Fabric::HARC_BLOCK_SIZE>*> Fabric::harcs__;

set<Node, bool(*)(const Node &, const Node &)> Fabric::unproc__(harcCompare);
std::mutex Fabric::unproc_lock__;

array<Node, Fabric::SIGNIFICANT_QUEUE_SIZE> Fabric::sigharcs__;
std::mutex Fabric::sigharcs_lock__;

std::atomic<size_t> Fabric::branchcount__(0);
std::atomic<size_t> Fabric::harccount__(1);
std::atomic<size_t> Fabric::activatecount__(0);
std::atomic<size_t> Fabric::followcount__(0);
std::atomic<size_t> Fabric::processed__(0);



void Fabric::counterThread() {
	while (true) {
		++counter__;

		std::this_thread::sleep_for(
				std::chrono::milliseconds(counterResolution()));
	}
}


namespace {
condition_variable proccv;
};

void Fabric::processThread() {
	while (true) {
		Node node;
		{
			unique_lock<mutex> lck(unproc_lock__);
			while (unproc__.size() == 0) {
				proccv.wait(lck);
			}
			node = *unproc__.begin();
			unproc__.erase(unproc__.begin());
		}

		++processed__;

		// Generate Tails.
		// FOR ALL COMBINATIONS!!!!!!
		Tail tail(SIGNIFICANT_QUEUE_SIZE);
		for (auto i : sigharcs__) {
			tail.insertRaw(i);
		}
		tail.fixup();
		define(tail, node);

		// Insert into sigharcs
		auto it = std::min_element(sigharcs__.begin(), sigharcs__.end(), harcMin);
		*it = node;
	}
}



void Fabric::initialise() {
	std::thread t(counterThread);
	t.detach();


	std::thread p(processThread);
	p.detach();

	for (size_t i = 0; i < SIGNIFICANT_QUEUE_SIZE; ++i) {
		sigharcs__[i] = dharc::null_n;
	}
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
	Harc *h = get(n);
	++activatecount__;
	h->activate(value);
	addToQueue(n, h);
}

void Fabric::activate(const Node &first,
						const Node &last,
						const vector<float> &amount) {
	const size_t count = last.value - first.value;
	activatecount__ += count;
	Node current = first;

	for (size_t i = 0; i < count; ++i) {
		Harc *h = get(current);
		h->activate(amount[i]);
		addToQueue(current, h);
		++current.value;
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
		branchcount__ += tail.size();
	}

	++followcount__;

	Harc *harc = get(hnode);
	harc->define(head);
	++activatecount__;
	addToQueue(hnode, harc);
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

void Fabric::addToQueue(const Node &node, Harc *harc) {
	if (harc->significance() < SIG_THRESHOLD) return;

	unique_lock<mutex> lck(unproc_lock__);
	if (unproc__.size() < MAX_UNPROCESSED) {
		unproc__.insert(node);
	} else {
		if (harcCompare(node, *unproc__.rbegin())) {
			unproc__.erase(--unproc__.end());
			unproc__.insert(node);
		}
	}
	proccv.notify_one();
}

