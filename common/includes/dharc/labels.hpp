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

	bool autoGenerate() const { return autogen_; }
	void autoGenerate(bool autogen) { autogen_ = autogen; }

	int get(const std::string &str);
	const std::string &get(int id);
	Node getNode(int id);
	Node getNode(const std::string &str);
	const std::string &getNode(const Node &node);

	bool exists(const std::string &str);

	bool set(int id, const std::string &str);

	private:
	bool autogen_;
	int lastid_;
	std::map<int, std::string> label_idtostr_;
	std::map<std::string, int> label_strtoid_;
};

extern Labels labels;

};  // namespace dharc

#endif  // DHARC_LABELS_HPP_

