/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/harc.hpp"

#include <vector>
#include <iostream>
#include <thread>

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

/* void Harc::reposition_harc(const list<Harc*> &p) {
	auto npos = p.begin();
	for (auto i : p) {}
}*/

void Harc::update_partners(const Nid &n) {
	int max = Fabric::sig_prop_max();
	auto partners = fabric.m_partners[n];
	
	// TODO(knicos): Find and reposition.
	partners.push_front(this);
	partners.unique();
	
	for (auto i : partners) {
		if (--max == 0) break;
		// TODO(knicos): Boost ch significance.
		auto p1 = fabric.m_partners[i->tail_other(n)];
		// TODO(knicos): Find and reposition.
		p1.push_front(i);
		p1.unique();
		auto p2 = fabric.m_partners[i->tail_other(n)];
		// TODO(knicos): Find and reposition.
		p2.push_front(i);
		p2.unique();
	}
}

const Nid &Harc::query() {
	// Boost significance
	update_partners(m_tail.first);
	update_partners(m_tail.second);
	
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
