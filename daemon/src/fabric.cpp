/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/fabric.hpp"

#include <future>
#include <thread>
#include <vector>
#include <utility>
#include <list>
#include <atomic>
#include <chrono>

using fdsb::Fabric;
using fdsb::Harc;
using fdsb::Nid;
using std::vector;
using std::list;
using std::atomic;

atomic<unsigned long long> Fabric::s_counter(0);

void Fabric::counter_thread() {
	while (true) {
		++s_counter;
		std::this_thread::sleep_for(
				std::chrono::milliseconds(counter_resolution()));
	}
}

Fabric &fdsb::fabric = Fabric::singleton();

Fabric::Fabric()
	: m_changes(new forward_list<Harc*>()) {
	std::thread t(counter_thread);
	t.detach();
}

Fabric::~Fabric() {
}

unique_ptr<forward_list<Harc*>> Fabric::changes() {
	unique_ptr<forward_list<Harc*>> newptr(new forward_list<Harc*>());
	m_changes.swap(newptr);
	return newptr;
}

void Fabric::log_change(Harc *h) {
	m_changes->push_front(h);
}

const list<Harc*> &Fabric::partners(const Nid &n) {
	return m_partners[n];
}

Harc &Fabric::get(const pair<Nid, Nid> &key) {
	auto it = m_harcs.find(key);

	if (it != m_harcs.end()) {
		return *(it->second);
	} else {
		auto h = new Harc(key);
		m_harcs.insert({key, h});

		// Update node partners to include this harc
		// TODO(knicos): This should be insertion sorted.
		auto &p1 = m_partners[key.first];
		p1.push_front(h);
		h->m_partix[0] = p1.begin();
		auto &p2 = m_partners[key.second];
		p2.push_front(h);
		h->m_partix[1] = p2.begin();

		return *h;
	}
}

Nid Fabric::path_s(const vector<Nid> &p, Harc *dep) {
	if (p.size() > 1) {
		Nid cur = p[0];

		if (dep) {
			for (auto i = ++p.begin(); i != p.end(); ++i) {
				Harc &h = get(cur, *i);
				h.add_dependant(*dep);
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

bool Fabric::path_r(const vector<vector<Nid>> &p, Nid *res,
	int s, int e, Harc *dep) {
	for (auto i = s; i < e; ++i) {
		res[i] = fabric.path_s(p[i], dep);
	}
	return true;
}

void Fabric::paths(const vector<vector<Nid>> &p, Nid *res, Harc *dep) {
	int size = p.size();

	// Should we divide the paths?
	if (size > 2 && pool_count > 0) {
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
	} else {
		for (auto i = 0; i < size; ++i) {
			res[i] = path_s(p[i], dep);
		}
	}
}

Nid Fabric::path(const vector<vector<Nid>> &p, Harc *dep) {
	vector<Nid> res(p.size());

	// Process all the sub paths
	paths(p, res.data(), dep);
	// Final recombination
	return path_s(res, dep);
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

