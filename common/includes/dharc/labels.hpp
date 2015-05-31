/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_LABELS_HPP_
#define DHARC_LABELS_HPP_

#include <string>
#include <map>

#include "dharc/node.hpp"

namespace dharc {

class Labels {
	public:
	Labels();
	~Labels();

	void dump(std::ostream &output);

	Node get(const std::string &str);
	const std::string &get(const Node &node);

	bool exists(const std::string &str);

	bool set(const Node &node, const std::string &str);

	private:
	std::map<Node, std::string> label_nodetostr_;
	std::map<std::string, Node> label_strtonode_;
};

extern Labels labels;

};  // namespace dharc

#endif  // DHARC_LABELS_HPP_

