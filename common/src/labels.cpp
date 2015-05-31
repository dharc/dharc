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

Labels::Labels() : autogen_(true), lastid_(10000) {
}

Labels::~Labels() {
}

int dharc::Labels::get(const string &str) {
	auto it = label_strtoid_.find(str);
	if (it != label_strtoid_.end()) {
		return it->second;
	}

	if (autogen_) {
		set(lastid_, str);
		return lastid_-1;
	}
	return -1;
}

const string &dharc::Labels::get(int id) {
	auto it = label_idtostr_.find(id);
	if (it != label_idtostr_.end()) {
		return it->second;
	}
	return not_a_label;
}

Node dharc::Labels::getNode(int id) {
	return Node(Node::Type::constant, id);
}

Node dharc::Labels::getNode(const string &str) {
	return Node(Node::Type::constant, get(str));
}

const string &dharc::Labels::getNode(const Node &node) {
	if (node.t == Node::Type::constant) {
		return get(node.i);
	} else {
		return not_a_label;
	}
}

bool Labels::exists(const std::string &str) {
	return (label_strtoid_.find(str) != label_strtoid_.end());
}

bool dharc::Labels::set(int id, const string &str) {
	if (label_idtostr_.find(id) != label_idtostr_.end()) return false;
	label_idtostr_.insert(std::pair<int, string>(id, str));
	label_strtoid_.insert(std::pair<string, int>(str, id));

	if (id >= lastid_) lastid_ = id+1;

	return true;
}

void Labels::dump(std::ostream &output) {
	for (auto i : label_idtostr_) {
		output << "%define " << i.second << " " << i.first << std::endl;
	}
}

