/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/definition.hpp"

#include <string>
#include <vector>
#include <thread>

#include "fdsb/fabric.hpp"

using fdsb::Nid;
using fdsb::Harc;
using fdsb::Definition;
using std::string;
using std::vector;

const Nid &Definition::evaluate(Harc *harc) {
	// Potentially unsafe if redefined before queried.
	while (m_outofdate) {
		// Can we update the definition
		if (!m_lock.test_and_set()) {
			m_cache = fabric.path(m_path, harc);
			m_outofdate = false;
			m_lock.clear();
		// Or must we wait on some other thread
		} else {
			// Do something else or yield
			std::this_thread::yield();
		}
	}
	return m_cache;
}

string Definition::to_string() const {
	return "";
}

vector<vector<Nid>> Definition::to_path() const {
	return {{}};
}

Definition *Definition::from_string(const string &str) {
	return nullptr;
}

Definition *Definition::from_path(const vector<vector<Nid>> &path) {
	Definition *res = new Definition();
	res->m_path = path;
	return res;
}
