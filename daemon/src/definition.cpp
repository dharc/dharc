/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/definition.hpp"

#include <string>
#include <vector>
#include <thread>
#include <iostream>

#include "dharc/fabric.hpp"

using dharc::Node;
using dharc::Harc;
using dharc::Definition;
using std::string;
using std::vector;
using std::istream;
using std::ostream;

Definition::Definition(const vector<vector<Node>> &definition) {
	path_ = definition;
	for (auto i : path_) {
		i.shrink_to_fit();
	}
	path_.shrink_to_fit();
}



Node Definition::evaluate(const Harc *harc) const {
	vector<Node> aggregate = fabric.paths(path_, harc);
	return fabric.path(aggregate, harc);
}



std::ostream &dharc::operator<<(std::ostream &os, const Definition &d) {
	os << '{';
	for (auto i : d.path_) {
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


