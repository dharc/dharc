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

#include "dharc/harc.hpp"

using dharc::Fabric;
using dharc::fabric::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;
using dharc::Tail;
using dharc::fabric::HarcMap;


atomic<unsigned long long> Fabric::counter__(0);

HarcMap                 Fabric::harcs__;
multimap<float, const Harc*>                                 Fabric::changes__;
unordered_map<Node, multimap<float,
		const Harc*>, Fabric::NidHash> Fabric::partners__;

std::atomic<size_t> Fabric::linkcount__(0);
std::atomic<size_t> Fabric::nodecount__(0);
std::atomic<size_t> Fabric::variablelinks__(0);
std::atomic<size_t> Fabric::changecount__(0);
std::atomic<size_t> Fabric::querycount__(0);


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



void Fabric::changes(vector<const Tail*>& vec, size_t count) {
	size_t ix = 0;
	for (auto i : changes__) {
		if (ix == count) break;
		vec[ix] = &i.second->tail();
	}
}



void Fabric::logChange(const Harc *h) {
	// Todo(knicos): Use lifo buffer ?
	changes__.insert(pair<float, const Harc*>(h->significance(), h));
}



void Fabric::partners(const Node& node, vector<Node>& vec,
							size_t count, size_t start) {
	multimap<float, const Harc*> &part = partners__[node];
	count = (part.size() > count) ? count : part.size();
	vec.resize(count);
	// size_t ix = 0;
	// TODO(knicos): To return Harcs or nodes. If nodes, which ones??
	// if harcs then need harc ids instead of pointers.
	/*for (auto i : part) {
		if (ix == count) break;
		vec[ix++] = i.second;
	}*/
}



void Fabric::add(Harc *h, const Tail &key) {
	h->setIterator(harcs__.insert({key, h}).first);

	++linkcount__;

	// Update node partners to include this harc
	for (auto i : h->tail()) {
		auto &p = partners__[i];
		// h->partix_[0] =
		p.insert({h->significance(), h});
	}
}



Harc &Fabric::get(const Tail &key) {
	Harc *h;
	if (get(key, h)) return *h;

	Tail key_sorted = key;
	std::sort(key_sorted.begin(), key_sorted.end());

	if (get(key_sorted, h)) return *h;

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

	add(h, key_sorted);
	return *h;
}



bool Fabric::get(const Tail &key, Harc*& result) {
	auto it = harcs__.find(key);

	if (it != harcs__.end()) {
		result = it->second;
		return true;
	}
	return false;
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

void Fabric::define(const Tail &tail, const vector<vector<Node>> &def) {
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



Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	if (p.size() > 1) {
		Node cur = p[0];

		if (dep) {
			for (auto i = ++p.begin(); i != p.end(); ++i) {
				Harc &h = get(cur, *i);
				h.addDependant(*dep);
				cur = h.query();
			}
		} else {
			for (auto i = ++p.begin(); i != p.end(); ++i) {
				cur = get(cur, *i).query();
			}
		}
		return cur;
	// Base case, should never really occur
	} else if (p.size() == 1) {
		return p[0];
	// Pointless
	} else {
		return null_n;
	}
}

std::atomic<int> pool_count(std::thread::hardware_concurrency());

bool Fabric::path_r(const vector<vector<Node>> &p, Node *res,
	int s, int e, const Harc *dep) {
	for (auto i = s; i < e; ++i) {
		res[i] = path(p[i], dep);
	}
	return true;
}

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
	for (auto i : h->tail()) {
		auto &p = partners__[i];
		// TODO(knicos): REPLACE EXISTING SOMEHOW.
		// p.erase(h->partix_[0]);
		p.insert({h->significance(), h});
	}

	// TODO(knicos): Propagate to partners of partners...
	// This could be done slowly in another thread.
}


