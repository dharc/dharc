/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/labels.hpp"

#include <map>
#include <utility>
#include <string>

using dharc::Node;
using std::string;
using std::map;
using dharc::Labels;

Labels dharc::labels;

namespace {
string not_a_label = "NAL";
};  // namespace

Labels::Labels() {
}

Labels::~Labels() {
}

Node dharc::Labels::get(const string &str) {
	auto it = label_strtonode_.find(str);
	if (it != label_strtonode_.end()) {
		return it->second;
	}
	return dharc::null_n;
}

const string &dharc::Labels::get(const Node &node) {
	auto it = label_nodetostr_.find(node);
	if (it != label_nodetostr_.end()) {
		return it->second;
	}
	return not_a_label;
}

bool Labels::exists(const std::string &str) {
	return (label_strtonode_.find(str) != label_strtonode_.end());
}

bool dharc::Labels::set(const Node &node, const string &str) {
	if (label_nodetostr_.find(node) != label_nodetostr_.end()) return false;
	label_nodetostr_.insert({node, str});
	label_strtonode_.insert({str, node});

	return true;
}

void Labels::dump(std::ostream &output) {
	for (auto i : label_nodetostr_) {
		output << "%define " << i.second << " " << i.first << std::endl;
	}
}

