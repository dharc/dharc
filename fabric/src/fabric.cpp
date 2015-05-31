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

#include "dharc/harc.hpp"

using dharc::Fabric;
using dharc::fabric::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;
using dharc::TailHash;
using dharc::NidHash;
using dharc::Tail;


atomic<unsigned long long> Fabric::counter__(0);

unordered_map<Tail, Harc*, TailHash>       Fabric::harcs__;
unique_ptr<forward_list<const Harc*>>      Fabric::changes__(
	new forward_list<const Harc*>());
unordered_map<Node, list<Harc*>, NidHash>  Fabric::partners__;

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



unique_ptr<forward_list<const Harc*>> Fabric::changes() {
	unique_ptr<forward_list<const Harc*>> newptr(
		new forward_list< const Harc*>());
	changes__.swap(newptr);
	return newptr;
}



void Fabric::logChange(const Harc *h) {
	changes__->push_front(h);
}

const list<Harc*> &Fabric::partners(const Node &n) {
	return partners__[n];
}



void Fabric::add(Harc *h) {
	harcs__.insert({h->tail(), h});

	++linkcount__;

	// Update node partners to include this harc
	// TODO(knicos): This should be insertion sorted.
	auto &p1 = partners__[h->tail().first];
	p1.push_front(h);
	h->partix_[0] = p1.begin();
	auto &p2 = partners__[h->tail().second];
	p2.push_front(h);
	h->partix_[1] = p2.begin();
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
		h = new Harc(key);
	//}

	add(h);
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
	get(tail).define(head);
}

void Fabric::define(const Tail &tail, const vector<vector<Node>> &def) {
	++changecount__;
	get(tail).define(def);
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


void Fabric::reposition(const list<Harc*> &p, list<Harc*>::iterator &it) {
	if (p.begin() == it) return;
	auto it2 = it--;
	std::swap(*it, *it2);

	// Brute force if IX not in meta data for harc.
	/* if (p.front() == this) return;
	for (auto i = ++p.begin(); i != p.end(); ++i) {
		if (*i == this) {
			// Move one place
			//auto i2 = i--;
			//std::swap(*i, *i2);
			//p.insert(--(p.erase(i)), this);
			return;
		}
	} */
}

void Fabric::updatePartners(const Node &n, list<Harc*>::iterator &it) {
	auto &partners = partners__[n];

	Harc *h = *it;
	partners.erase(it);

	// Insertion sort this harc by significance value
	// Most likely very near the front of the list
	auto j = partners.begin();
	while (j != partners.end()) {
		if ((*j)->significance() <= h->significance()) {
			it = partners.insert(j, h);
			break;
		}
		++j;
	}

	if (j == partners.end()) {
		partners.push_back(h);
		it = --partners.end();
	}

	// Shift all other partners up one place
	// TODO(knicos): In another thread.
	for (auto i : partners) {
		if (i == h) continue;  // Ignore self.

		if (i->tail().first == n) {
			auto &p = partners__[i->tail().second];
			reposition(p, i->partix_[1]);
		} else {
			auto &p = partners__[i->tail().first];
			reposition(p, i->partix_[0]);
		}
	}
}


