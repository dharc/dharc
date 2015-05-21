/*
 * Copyright 2015 Nicolas Pope
 */

#include "fdsb/definition.hpp"

#include <string>
#include <vector>
#include <thread>
#include <sstream>

#include "fdsb/fabric.hpp"

using fdsb::Nid;
using fdsb::Harc;
using fdsb::Definition;
using std::string;
using std::vector;
using std::stringstream;

const Nid &Definition::evaluate(Harc *harc) const {
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
	stringstream res;
	res << *this;
	return res.str();
}

std::ostream &fdsb::operator<<(std::ostream &os, const Definition &d) {
	os << '{';
	for (auto i : d.m_path) {
		os << '(';
		for (auto j : i) {
			os << j << ' ';
		}
		os.seekp(-1, std::ios_base::end);
		os << ')';
	}
	os << '}';
	return os;
}

vector<vector<Nid>> Definition::to_path() const {
	return m_path;
}

static void remove_ws(stringstream &s) {
	char c;
	while ((c = s.peek()) != EOF && (c == ' ' || c == '\t')) s.ignore();
}

static Nid parse_nid(stringstream &s) {
	string tmp = "";
	char c;

	while ((c = s.peek()) != EOF
			&& c != ' '
			&& c != '\t'
			&& c != ')'
			&& c != '}') {
		tmp += c;
		s.ignore();
	}

	return Nid::from_string(tmp);
}

static vector<Nid> parse_path(stringstream &s) {
	char c;
	vector<Nid> res;

	while ((c = s.peek()) != EOF && c != ')' && c != '}') {
		res.push_back(parse_nid(s));
		remove_ws(s);
	}

	return res;
}

Definition *Definition::from_string(const string &str) {
	stringstream s(str);
	char c;

	remove_ws(s);
	if (s.get() != '{') return nullptr;

	vector<vector<Nid>> path;

	do {
		remove_ws(s);
		c = s.peek();
		if (c == ')') {
			return nullptr;
		} else if (c == '(') {
			s.ignore();
			remove_ws(s);
			path.push_back(parse_path(s));
			if (s.get() != ')') return nullptr;
		} else {
			path.push_back({parse_nid(s)});
		}
		remove_ws(s);
	} while ((c = s.peek()) != EOF && c != '}');

	if (c == EOF) return nullptr;


	return from_path(path);
}

Definition *Definition::from_path(const vector<vector<Nid>> &path) {
	Definition *res = new Definition();
	res->m_path = path;
	return res;
}
