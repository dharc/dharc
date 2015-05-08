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
			m_head = fabric.path(m_def->def, this);
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
	fabric.log_change(this);
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
	fabric.log_change(this);
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
