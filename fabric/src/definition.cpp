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
using dharc::fabric::Harc;
using dharc::Definition;
using std::string;
using std::vector;
using std::istream;
using std::ostream;

Definition::Definition(const vector<Node> &definition) {
	path_ = definition;
	path_.shrink_to_fit();
}



Node Definition::evaluate(const Harc *harc) const {
	return Fabric::path(path_, harc);
}



vector<Node> Definition::instantiate(const Node &any) {
	vector<Node> result = path_;
	/*for (auto i = result.begin(); i != result.end(); ++i) {
		for (auto j = i->begin(); j != i->end(); ++j) {
			if (*j == dharc::any_n) {
				*j = any;
			}
		}
	}*/
	return result;
}



std::ostream &dharc::operator<<(std::ostream &os, const Definition &d) {
	os << '{';
	for (auto i : d.path_) {
		os << i << ' ';
	}
	os.seekp(-1, std::ios_base::cur);
	os << '}';
	return os;
}


