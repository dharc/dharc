/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/harc.hpp"

#include <thread>
#include <vector>
#include <future>
#include <iostream>

#include "fdsb/nid.hpp"

using fdsb::Harc;
using fdsb::Nid;

std::unordered_multimap<unsigned long long, Harc*> Harc::s_fabric(100000);

Harc::Harc(const Nid &a, const Nid &b) :
	m_head(null_n),
	m_def(nullptr) {
	m_tail[0] = a;
	m_tail[1] = b;
}

void Harc::add_dependant(Harc &h) {
	m_dependants.push_back(&h);
}

const Nid &Harc::query() {
	while (m_def && m_def->outofdate) {
		// Can we update the definition
		if (!m_def->lock.test_and_set()) {
			m_head = path(m_def->def, this);
			m_def->outofdate = false;
			m_def->lock.clear();
		// Or must we wait on some other thread
		} else {
			// Do something else or yield
			std::this_thread::yield();
		}
	}
	return m_head;
}

void Harc::dirty() {
	if (m_def) m_def->outofdate = true;
	for (auto i : m_dependants) {
		i->dirty();
	}
	m_dependants.clear();
}

void Harc::define(const Nid &n) {
	m_head = n;
	if (m_def) {
		delete m_def;
		m_def = nullptr;
	}
	for (auto i : m_dependants) {
		i->dirty();
	}
	m_dependants.clear();
}

void Harc::define(const fdsb::Path &p) {
	if (m_def) delete m_def;
	m_def = new Definition(p);
	dirty();
}

Harc &Harc::operator=(const Nid &n) {
	define(n);
	return *this;
}

bool Harc::operator==(const Nid &n) {
	return query() == n;
}

Harc &Nid::operator[](const Nid &n) {
	return Harc::get(*this, n);
}

Harc &Harc::operator[](const Nid &n) {
	return get(query(), n);
}

Harc &Harc::get(const Nid &a, const Nid &b) {
	Harc *h;
	auto range = s_fabric.equal_range(Nid::dual_hash(a, b));

	// Find the exact Harc in the bucket.
	for (auto i = range.first; i != range.second; ++i) {
		h = i->second;
		if (h->equal_tail(a, b)) {
			return *h;
		}
	}

	// Does not exist, so make it.
	h = new Harc(a, b);
	s_fabric.insert({{Nid::dual_hash(a, b), h}});
	return *h;
}

Nid Harc::path_s(const std::vector<Nid> &p, Harc *dep) {
	if (p.size() > 1) {
		Nid cur = p[0];

		if (dep) {
			for (auto i = ++p.begin(); i != p.end(); ++i) {
				Harc &h = Harc::get(cur, *i);
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

bool Harc::path_r(const fdsb::Path &p, Nid *res,
	int s, int e, Harc *dep) {
	for (auto i = s; i < e; ++i) {
		res[i] = path_s(p[i], dep);
	}
	return true;
}

void Harc::paths(const fdsb::Path &p, Nid *res, Harc *dep) {
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

Nid Harc::path(const fdsb::Path &p, Harc *dep) {
	std::vector<Nid> res(p.size());

	// Process all the sub paths
	paths(p, res.data(), dep);
	// Final recombination
	return Harc::path_s(res, dep);
}
