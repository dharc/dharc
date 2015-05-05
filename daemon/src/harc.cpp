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

std::unordered_multimap<unsigned long long, Harc*> Harc::s_fabric;

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

void Harc::define(const std::vector<std::vector<Nid>> &p) {
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

	for (auto i = range.first; i != range.second; i++) {
		h = i->second;
		if (h->equal_tail(a, b)) {
			return *h;
		}
	}

	h = new Harc(a, b);
	s_fabric.insert({{Nid::dual_hash(a, b), h}});
	return *h;
}

Nid Harc::path_s(const std::vector<Nid> &p, Harc *dep) {
	if (p.size() > 1) {
		Nid temp = p[0];
		for (auto i = ++p.begin(); i != p.end(); ++i) {
			Harc &h = Harc::get(temp, *i);
			if (dep) {
				h.add_dependant(*dep);
			}
			temp = h.query();
		}
		return temp;
	} else if (p.size() == 1) {
		return p[0];
	} else {
		return null_n;
	}
}

std::atomic<int> pool_count(4);

Nid Harc::path(const std::vector<std::vector<Nid>> &p, Harc *dep) {
	std::vector<Nid> res(p.size());

	// Should we attempt to split the load?
	if (p.size() > 2 && pool_count > 0) {
		--pool_count;

		// Divide paths into 2 parts.
		auto middle = p.size() / 2;

		// Asynchronously perform second half
		std::future<bool> fa = std::async(
			std::launch::async,
			[](const std::vector<std::vector<Nid>> &p, Nid *res,
					int s, int e, Harc *dep) {
				for (auto i = s; i < e; ++i) {
					res[i] = Harc::path_s(p[i], dep);
				}
				return true;
			}, p, res.data(), middle, p.size(), dep);

		// Calculate first half
		Nid cur = Harc::path_s(p[0], dep);
		for (unsigned int i = 1; i < middle; ++i) {
			Harc &h = Harc::get(cur, Harc::path_s(p[i], dep));
			if (dep) {
				h.add_dependant(*dep);
			}
			cur = h.query();
		}

		// Sync with other thread.
		fa.get();
		++pool_count;

		// Combine async results with first half
		for (unsigned int i = middle; i < p.size(); ++i) {
			Harc &h = Harc::get(cur, res[i]);
			if (dep) {
				h.add_dependant(*dep);
			}
			cur = h.query();
		}
		return cur;
	} else {
		int ix = 0;

		for (auto i : p) {
			res[ix++] = path_s(i, dep);
		}

		// Final recombination
		return Harc::path_s(res, dep);
	}
}
