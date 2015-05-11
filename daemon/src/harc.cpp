/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/harc.hpp"

#include <vector>
#include <iostream>
#include <thread>
#include <list>
#include <utility>
#include <algorithm>

#include "fdsb/nid.hpp"
#include "fdsb/fabric.hpp"

using fdsb::Harc;
using fdsb::Fabric;
using fdsb::Nid;

Harc::Harc(const pair<Nid, Nid> &t) :
	m_tail(t),
	m_head(null_n),
	m_flags(Flag::none) {}

void Harc::add_dependant(Harc &h) {
	m_dependants.push_back(&h);
}

void Harc::reposition_harc(list<Harc*> &p, list<Harc*>::iterator &it) {
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

void Harc::update_partners(const Nid &n, list<Harc*>::iterator &it) {
	int max = Fabric::sig_prop_max();
	auto &partners = fabric.m_partners[n];

	reposition_harc(partners, it);

	for (auto i : partners) {
		if (--max == 0) break;
		if (i->tail().first == n) {
			auto &p1 = fabric.m_partners[i->tail().second];
			reposition_harc(p1, i->m_partix[1]);
		} else {
			auto &p1 = fabric.m_partners[i->tail().first];
			reposition_harc(p1, i->m_partix[0]);
		}
	}
}

const Nid &Harc::query() {
	// Boost significance
	update_partners(m_tail.first, m_partix[0]);
	update_partners(m_tail.second, m_partix[1]);

	if (check_flag(Flag::defined)) {
		// Potentially unsafe if redefined before queried.
		while (m_def->outofdate) {
			// Can we update the definition
			if (!m_def->lock.test_and_set()) {
				m_def->cache = fabric.path(m_def->def, this);
				m_def->outofdate = false;
				m_def->lock.clear();
			// Or must we wait on some other thread
			} else {
				// Do something else or yield
				std::this_thread::yield();
			}
		}
		return m_def->cache;
	} else {
		return m_head;
	}
}

void Harc::dirty() {
	if (check_flag(Flag::defined)) m_def->outofdate = true;
	for (auto i : m_dependants) {
		i->dirty();
	}
	m_dependants.clear();
}

void Harc::define(const Nid &n) {
	if (check_flag(Flag::log)) fabric.log_change(this);

	if (check_flag(Flag::defined)) {
		delete m_def;
		clear_flag(Flag::defined);
	}

	m_head = n;

	for (auto i : m_dependants) {
		i->dirty();
	}
	m_dependants.clear();
}

void Harc::define(const fdsb::Path &p) {
	if (check_flag(Flag::defined)) {
		delete m_def;
	} else {
		set_flag(Flag::defined);
	}
	m_def = new Definition(p);

	if (check_flag(Flag::log)) fabric.log_change(this);

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
	return fabric.get(*this, n);
}

Harc &Harc::operator[](const Nid &n) {
	return fabric.get(query(), n);
}
