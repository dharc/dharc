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
using fdsb::Nid;

Harc::Harc() :
	m_head(null_n),
	m_flags(Flag::none) {}

void Harc::add_dependant(Harc &h) {
	m_dependants.push_back(&h);
}

const Nid &Harc::query() {
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
