/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_ARCH_SCRIPT_HPP_
#define DHARC_ARCH_SCRIPT_HPP_

#include <vector>
#include <map>
#include <string>
#include <list>

#include "dharc/node.hpp"
#include "dharc/parse.hpp"

namespace dharc {
namespace arch {

class Script {
	dharc::parser::Context ctx_;
	const char *source_;
	const std::vector<dharc::Node> *params_;
	bool hasresult_;
	bool printmessages_;

	public:
	Script() = delete;
	explicit Script(std::istream &is, const char *src = nullptr);

	dharc::Node operator()() { return (*this)(std::vector<dharc::Node>{}); }
	dharc::Node operator()(const std::vector<dharc::Node> &p);

	inline void showInformation(bool b) { ctx_.showInformation(b); }
	inline void showMessages(bool b) { printmessages_ = b; }

	const std::list<dharc::parser::Message> &messages() {
		return ctx_.messages();
	}

	bool hasResult() const { return hasresult_; }

	private:
	void parseStatement(dharc::Node &cur);
	bool parseNode(dharc::Node &val);
	void parseDefinition(std::vector<std::vector<dharc::Node>> &def);
	void commandDefine();
	void commandLabel();
};

};  // namespace arch
};  // namespace dharc

#endif  // DHARC_ARCH_SCRIPT_HPP_

