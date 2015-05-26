/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_SCRIPT_H_
#define DHARC_SCRIPT_H_

#include <vector>

#include "dharc/nid.hpp"
#include "dharc/parse.hpp"

namespace dharc {

class Script {
	dharc::parser::Context ctx;
	const char *source;
	const std::vector<dharc::Nid> *params;

	public:
	Script() = delete;
	explicit Script(std::istream &is, const char *src = nullptr);

	dharc::Nid operator()() { return (*this)(std::vector<dharc::Nid>{}); }
	dharc::Nid operator()(const std::vector<dharc::Nid> &p);

	inline void show_info(bool b) { ctx.show_info(b); }

	private:
	void parse_statement(dharc::Nid &cur, dharc::parser::Context &parse);
	bool parse_node(dharc::parser::Context &ctx, dharc::Nid &value);
};

};  // namespace dharc

#endif  /* DHARC_SCRIPT_H_ */

