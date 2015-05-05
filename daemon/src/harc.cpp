/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/harc.hpp"

#include <thread>
#include <vector>
#include <future>

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

Nid Harc::path(const std::vector<std::vector<Nid>> &p, Harc *dep) {
	int ix = 0;
	auto fut = new std::future<Nid>[p.size()];
	std::vector<Nid> res(p.size());

	// These can all be done in different threads!
	for (auto i : p) {
		fut[ix++] = std::async(std::launch::async, Harc::path_s, i, dep);
	}
	// Final recombination
	for (unsigned int i = 0; i < p.size(); ++i) {
		res[i] = fut[i].get();
	}

	delete [] fut;
	return Harc::path_s(res, dep);
}
