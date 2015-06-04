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

#include "dharc/harc.hpp"

using dharc::Fabric;
using dharc::fabric::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;
using dharc::Tail;
using dharc::fabric::HarcMap;
using dharc::fabric::SortedHarcs;


atomic<unsigned long long> Fabric::counter__(0);

HarcMap Fabric::harcs__(100000);
vector<const Harc *> Fabric::changes__;
unordered_map<Node, SortedHarcs, Fabric::NidHash> Fabric::partners__;

std::atomic<size_t> Fabric::linkcount__(0);
std::atomic<size_t> Fabric::nodecount__(0);
std::atomic<size_t> Fabric::variablelinks__(0);
std::atomic<size_t> Fabric::changecount__(0);
std::atomic<size_t> Fabric::querycount__(0);

std::mutex Fabric::changelock_;


/*
 * Used for sorting vectors of harcs, most significant first
 */
inline bool harc_sig_comp(const Harc *i, const Harc *j) {
	return i->significance() > j->significance();
}


void Fabric::counterThread() {
	while (true) {
		++counter__;

		// Cull the change log if it needs it.
		/*if (changes__.size() >= 2 * maxChanges()) {
			changelock_.lock();
			// Must sort before emptying
			// Or not and assume things at front are more important
			// std::partial_sort(changes__.begin(),
			//	 	changes__.begin() + maxChanges(),
			// 		changes__.end(), harc_sig_comp);
			changes__.resize(maxChanges());
			changelock_.unlock();
		}*/

		// TODO(knicos): Reduce sleep time by processing time.

		std::this_thread::sleep_for(
				std::chrono::milliseconds(counterResolution()));
	}
}



void Fabric::initialise() {
	std::thread t(counterThread);
	t.detach();

	changes__.reserve(maxChanges()*2);
}



void Fabric::finalise() {
}



void Fabric::changes(vector<const Tail*>& vec, size_t count) {
	changelock_.lock();
	count = (changes__.size() > count) ? count : changes__.size();
	vec.resize(count);
	size_t ix = 0;
	for (auto i : changes__) {
		if (ix == count) break;
		vec[ix] = &i->tail();
	}
	changelock_.unlock();
}



void Fabric::logChange(const Harc *h) {
	// TODO(knicos): Make sure change significance always increases?
	// makes sure most recent changes are at top.
	changelock_.lock();
	if (changes__.empty()) {
		changes__.push_back(h);
	} else if (h->significance() > changes__.back()->significance()) {
		changes__.push_back(h);
		std::sort(changes__.begin(), changes__.end(), harc_sig_comp);
		auto it = std::unique(changes__.begin(), changes__.end());
		auto dist = std::distance(changes__.begin(), it);
		if (dist > maxChanges()) dist = maxChanges();
		changes__.resize(dist);
	}
	changelock_.unlock();
}



void Fabric::partners(const Node& node, vector<const Tail *>& vec,
							size_t count, size_t start) {
	SortedHarcs &part = partners__[node];
	// Do the sort now
	std::sort(part.begin(), part.end(), harc_sig_comp);

	count = (part.size() > count) ? count : part.size();
	vec.resize(count);
	size_t ix = 0;
	for (auto i : part) {
		if (ix == count) break;
		vec[ix++] = &i->tail();
	}
}



void Fabric::add(Harc *h, const Tail &key) {
	h->setIterator(harcs__.insert({key, h}).first);

	++linkcount__;

	// Update node partners to include this harc
	for (auto i : h->tail()) {
		auto &p = partners__[i];
		// h->partix_[0] =
		p.push_back(h);
	}
}



Harc &Fabric::get(const Tail &key) {
	Harc *h;
	if (get(key, h)) return *h;

	// Check for an Any($) entry
	/*if (get(dharc::any_n, key.first, h)) {
		Harc *oldh = h;
		h = h->instantiate(key.second);
		if (oldh == h) return *h;
	} else if (get(dharc::any_n, key.second, h)) {
		Harc *oldh = h;
		h = h->instantiate(key.first);
		if (oldh == h) return *h;
	} else {*/
		h = new Harc();
	//}

	add(h, key);
	return *h;
}



bool Fabric::get(const Tail &key, Harc*& result) {
	auto it = harcs__.find(key);

	if (it != harcs__.end()) {
		result = it->second;
		// TODO(knicos): If harc ptr == nullptr, load from disk
		return true;
	}
	return false;
}



inline Node Fabric::queryFast(const Tail &tail) {
	Harc *h;
	return get(tail, h) ? h->query() : dharc::null_n;
}



Node Fabric::query(const Tail &tail) {
	++querycount__;
	return get(tail).query();
}

void Fabric::define(const Tail &tail, const Node &head) {
	++changecount__;
	Harc &h = get(tail);
	h.define(head);
	logChange(&h);
}

void Fabric::define(const Tail &tail, const vector<Node> &def) {
	++changecount__;
	Harc &h = get(tail);
	h.define(def);
	logChange(&h);
}



Node Fabric::unique() {
	return Node(nodecount__.fetch_add(1));
}



void Fabric::unique(int count, Node &first, Node &last) {
	first.value = nodecount__.fetch_add(count);
	last.value = first.value + count - 1;
}



Node Fabric::path(const vector<Node> &p, size_t &index,
					size_t count, const Harc *dep) {
	Tail tail(count);
	while ((count--) && (index < p.size())) {
		if (p[index].isReserved()) {
			size_t tcount = p[index].reservedValue();
			tail.insert(path(p, ++index, tcount, dep));
		} else {
			tail.insert(p[index++]);
		}
	}
	if (dep) {
		Harc &h = get(tail);
		h.addDependant(*dep);
		return h.query();
	} else {
		return queryFast(tail);
	}
}



Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	size_t index = 0;
	Tail tail;
	while (index < p.size()) {
		if (p[index].isReserved()) {
			size_t tcount = p[index].reservedValue();
			tail.insert(path(p, ++index, tcount, dep));
		} else {
			tail.insert(p[index++]);
		}
	}
	if (dep) {
		Harc &h = get(tail);
		h.addDependant(*dep);
		return h.query();
	} else {
		return queryFast(tail);
	}
}

std::atomic<int> pool_count(std::thread::hardware_concurrency());



vector<Node> Fabric::paths(const vector<vector<Node>> &p, const Harc *dep) {
	vector<Node> result(p.size());
	int size = p.size();

	// Should we divide the paths?
	/*if (size > 2 && pool_count > 0) {
		--pool_count;

		// Divide paths into 2 parts.
		auto middle = size / 2;

		// Asynchronously perform first half
		std::future<bool> fa = std::async(
			std::launch::async,
			path_r, p, res, middle, size, dep);

		// Perform second half
		path_r(p, res, 0, middle, dep);

		// Sync results
		++pool_count;  // Add myself to the pool.
		fa.get();
	} else {*/
		for (auto i = 0; i < size; ++i) {
			result[i] = path(p[i], dep);
		}
	//}
	return result;
}



void Fabric::updatePartners(const Harc *h) {
	// NOTE(knicos): Partner sort is lazy, so this does not need updating

	/*for (auto i : h->tail()) {
		auto &p = partners__[i];
		// TODO(knicos): REPLACE EXISTING SOMEHOW.
		// p.erase(h->partix_[0]);
		if (std::find(p.begin(), p.end(), h) == p.end()) {
			p.push_back(h);
		}
	}*/

	// TODO(knicos): Propagate to partners of partners...
	// This could be done slowly in another thread.
}


