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
	bool showinfo;

	public:
	Script() = delete;
	explicit Script(std::istream &is, const char *src = nullptr);

	dharc::Nid operator()() { return (*this)(std::vector<dharc::Nid>{}); }
	dharc::Nid operator()(const std::vector<dharc::Nid> &params);

	inline void show_info(bool b) { showinfo = b; }
};

};  // namespace dharc

#endif  /* DHARC_SCRIPT_H_ */

