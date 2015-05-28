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
using dharc::Harc;
using dharc::Node;
using std::vector;
using std::list;
using std::atomic;

atomic<unsigned long long> Fabric::counter__(0);

void Fabric::counterThread() {
	while (true) {
		++counter__;
		std::this_thread::sleep_for(
				std::chrono::milliseconds(counterResolution()));
	}
}

Fabric &dharc::fabric = Fabric::singleton();

Fabric::Fabric()
	: changes_(new forward_list<const Harc*>()) {
	std::thread t(counterThread);
	t.detach();
}

Fabric::~Fabric() {
}

unique_ptr<forward_list<const Harc*>> Fabric::changes() {
	unique_ptr<forward_list<const Harc*>> newptr(
		new forward_list< const Harc*>());
	changes_.swap(newptr);
	return newptr;
}

void Fabric::logChange(const Harc *h) {
	changes_->push_front(h);
}

const list<Harc*> &Fabric::partners(const Node &n) {
	return partners_[n];
}

Harc &Fabric::get(const pair<Node, Node> &key) {
	auto it = harcs_.find(key);

	if (it != harcs_.end()) {
		return *(it->second);
	} else {
		auto h = new Harc(key);
		harcs_.insert({key, h});

		// Update node partners to include this harc
		// TODO(knicos): This should be insertion sorted.
		auto &p1 = partners_[key.first];
		p1.push_front(h);
		h->partix_[0] = p1.begin();
		auto &p2 = partners_[key.second];
		p2.push_front(h);
		h->partix_[1] = p2.begin();

		return *h;
	}
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
	} else if (p.size() == 1) {
		return p[0];
	} else {
		return null_n;
	}
}

std::atomic<int> pool_count(std::thread::hardware_concurrency());

bool Fabric::path_r(const vector<vector<Node>> &p, Node *res,
	int s, int e, const Harc *dep) {
	for (auto i = s; i < e; ++i) {
		res[i] = fabric.path(p[i], dep);
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
	auto &partners = partners_[n];

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
			auto &p = partners_[i->tail().second];
			reposition(p, i->partix_[1]);
		} else {
			auto &p = partners_[i->tail().first];
			reposition(p, i->partix_[0]);
		}
	}
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

