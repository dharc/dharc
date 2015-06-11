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

unordered_map<Tail, Node> Fabric::tails__(1000000);
vector<Fabric::HarcBlock*> Fabric::harcs__;
std::mutex Fabric::harc_lock__;

set<Node, bool(*)(const Node &, const Node &)> Fabric::unproc__(harcCompare);
std::mutex Fabric::unproc_lock__;

std::atomic<size_t> Fabric::branchcount__(0);
std::atomic<size_t> Fabric::harccount__(1);
std::atomic<size_t> Fabric::cullcount__(0);
std::atomic<size_t> Fabric::activatecount__(0);
std::atomic<size_t> Fabric::followcount__(0);
std::atomic<size_t> Fabric::processed__(0);

namespace {
size_t lastclean = 0;
unordered_map<Tail, Node>::iterator lastit;
vector<Node> freed;
};  // namespace

void Fabric::counterThread() {
	lastit = tails__.begin();

	while (true) {
		++counter__;

		if (counter__ - lastclean > GARBAGE_DELAY) {
			lastclean = counter__;

			// Do a garbage collect!!
			unique_lock<mutex> lck(harc_lock__);

			if (lastit == tails__.end()) {
				lastit = tails__.begin();
			}

			int maxgarbage = MAX_GARBAGE_CHUNK;
			for (; lastit != tails__.end(); ++lastit) {
				if (maxgarbage == 0) break;
				--maxgarbage;
				
				Harc *h = get((*lastit).second);
				if (h-> isWeak()) {
					freed.push_back((*lastit).second);
					(*lastit).second = dharc::null_n;
					h->reset();
					++cullcount__;
				}
			}

			std::cout << "Garbage: " << cullcount__ << "\n";
		} else {
			std::this_thread::sleep_for(
				std::chrono::milliseconds(counterResolution()));
		}
	}
}


namespace {
condition_variable proccv;
};

void Fabric::processThread() {
	array<Node, MAX_TAIL> signodes;
	Tail tail(MAX_TAIL);
	int count = 0;

	while (true) {
		// First select MAX_TAIL significant nodes
		{
			// Wait for lock and condition
			unique_lock<mutex> lck(unproc_lock__);
			while (unproc__.size() < 3) {
				proccv.wait(lck);
			}

			// Find max available if less than MAX_TAIL
			count = (unproc__.size() < MAX_TAIL) ? unproc__.size() : MAX_TAIL;

			// Copy the nodes to a buffer for processing
			auto it = unproc__.begin();
			for (int i = 0; i < count; ++i) {
				signodes[i] = *it;
				++it;
			}
			// Erase one from process queue.
			unproc__.erase(unproc__.begin());
		}

		++processed__;

		// Generate all tail combinations
		// First pick head node
		for (int n = 0; n < count; ++n) {
			// Vary number of tails
			for (int t = 0; t < (count - 2); ++t) {
				tail.reset();
				// Now pick t most significant 
				for (int i = 0; i < (count-t); ++i) {
					if (i == n) continue;
					tail.insertRaw(signodes[i]);
				}
				tail.fixup();
				query(tail, signodes[n]);
			}
		}

		// Insert into sigharcs
		//auto it = std::min_element(sigharcs__.begin(), sigharcs__.end(), harcMin);
		//*it = node;
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



Node Fabric::get(const Tail &key) {
	auto it = tails__.find(key);

	if (it != tails__.end()) {
		return it->second;
	}
	return dharc::null_n;
}


void Fabric::activateConstant(const Node &n, float value) {
	Harc *h = get(n);
	++activatecount__;
	h->activateConstant(value);
	addToQueue(n, h);
}

void Fabric::activateConstant(const Node &first,
						const Node &last,
						const vector<float> &amount) {
	const size_t count = last.value - first.value;
	activatecount__ += count;
	Node current = first;

	for (size_t i = 0; i < count; ++i) {
		Harc *h = get(current);
		h->activateConstant(amount[i]);
		addToQueue(current, h);
		++current.value;
	}
}

void Fabric::activatePulse(const Node &n) {
	Harc *h = get(n);
	++activatecount__;
	h->activatePulse();
	addToQueue(n, h);
}



/* Node Fabric::query(const Tail &tail) {
	++querycount__;
	return get(tail).query();
} */

Node Fabric::query(const Tail &tail, const Node &head) {
	Node hnode = get(tail);

	if (hnode == dharc::null_n) {
		harc_lock__.lock();
		hnode = makeHarc();
		tails__.insert({tail, hnode});
		harc_lock__.unlock();
		branchcount__ += tail.size();
	}

	++followcount__;
	Harc *h = get(hnode);
	if (h->query(head)) {
		activatePulse(hnode);
	}
	return hnode; 
}



Node Fabric::makeHarc() {
	if (freed.size() > 0) {
		Node node = freed.back();
		freed.pop_back();
		get(node)->notAvailable();
		--cullcount__;
		return node;
	}

	Node node = Node(harccount__.fetch_add(1));
	const auto x = node.value / HARC_BLOCK_SIZE;

	if (x >= harcs__.size()) {
		harcs__.push_back(new HarcBlock);
		harcs__[x]->harcs[0].notAvailable();
	}
	return node;
}



void Fabric::makeHarcs(int count, Node &first, Node &last) {
	first.value = harccount__.fetch_add(count);
	last.value = first.value + count - 1;

	for (int i = 0; i < count; i++) {
		const auto x = (first.value + i) / HARC_BLOCK_SIZE;
		if (x >= harcs__.size()) {
			harcs__.push_back(new HarcBlock);
		}
		const auto y = (first.value + i) % HARC_BLOCK_SIZE;
		harcs__[x]->harcs.at(y).notAvailable();
	}
}

void Fabric::addToQueue(const Node &node, Harc *harc) {
	if (harc->significance() < SIG_THRESHOLD) return;

	unique_lock<mutex> lck(unproc_lock__);
	if (unproc__.size() < MAX_UNPROCESSED) {
		unproc__.insert(node);
	} else {
		if (harcCompare(node, *(--unproc__.end()))) {
			unproc__.erase(--unproc__.end());
			unproc__.insert(node);
		}
	}
	proccv.notify_one();
}

