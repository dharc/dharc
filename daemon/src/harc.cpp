/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/harc.hpp"

#include <vector>
#include <iostream>
#include <thread>
#include <list>
#include <utility>
#include <algorithm>
#include <future>

#include "dharc/nid.hpp"
#include "dharc/fabric.hpp"
#include "dharc/definition.hpp"

using dharc::Harc;
using dharc::Fabric;
using dharc::Nid;
using dharc::Definition;

Harc::Harc(const pair<Nid, Nid> &t) :
	m_tail(t),
	m_head(null_n),
	m_flags(Flag::none),
	m_lastquery(Fabric::counter()),
	m_strength(0.0) {}

void Harc::add_dependant(const Harc &h) {
	m_dependants.push_back(&h);
}

void Harc::reposition_harc(const list<Harc*> &p, list<Harc*>::iterator &it) {
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

float Harc::significance() const {
	float delta = static_cast<float>(Fabric::counter()) -
						static_cast<float>(m_lastquery);
	if (delta == 0) delta = 1.0;
	return 1.0 / delta;
}

float Harc::last_query() const {
	float delta = static_cast<float>(Fabric::counter()) -
						static_cast<float>(m_lastquery);
	return (delta * Fabric::counter_resolution()) / 1000.0f;
}

void Harc::update_partners(const Nid &n, list<Harc*>::iterator &it) {
	auto &partners = fabric.m_partners[n];

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
			auto &p = fabric.m_partners[i->tail().second];
			reposition_harc(p, i->m_partix[1]);
		} else {
			auto &p = fabric.m_partners[i->tail().first];
			reposition_harc(p, i->m_partix[0]);
		}
	}
}

const Nid &Harc::query() {
	// Boost significance
	if (m_lastquery < Fabric::counter()) {
		m_lastquery = Fabric::counter();
		update_partners(m_tail.first, m_partix[0]);
		update_partners(m_tail.second, m_partix[1]);
	}

	if (check_flag(Flag::defined)) {
		return m_def->evaluate(this);
	} else {
		return m_head;
	}
}

const Nid &Harc::query() const {
	if (check_flag(Flag::defined)) {
		return m_def->evaluate(this);
	} else {
		return m_head;
	}
}

void Harc::dirty() const {
	if (check_flag(Flag::defined)) m_def->mark();
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

void Harc::define(const vector<vector<Nid>> &p) {
	if (check_flag(Flag::defined)) {
		delete m_def;
	} else {
		set_flag(Flag::defined);
	}
	m_def = Definition::from_path(p);

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

Nid Nid::operator[](const Nid &n) {
	return fabric.get(*this, n).query();
}

Harc &Harc::operator[](const Nid &n) {
	return fabric.get(query(), n);
}

std::ostream &dharc::operator<<(std::ostream &os, const Harc &h) {
	os << '[' << h.tail().first << ',' << h.tail().second
		<< "->" << h.query() << ']';
	return os;
}
