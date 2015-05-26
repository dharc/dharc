/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/script.hpp"

#include <vector>
#include <iostream>

#include "dharc/nid.hpp"
#include "dharc/parse.hpp"
#include "dharc/arch.hpp"

using dharc::Nid;
using dharc::parser::Context;
using std::vector;
using std::cout;
using dharc::parser::word_;
using dharc::parser::value_;
using dharc::parser::noact;

dharc::Script::Script(std::istream &is, const char *src)
: ctx(is), source(src) {
}

namespace {
struct command_query {
	Context &parse;

	void operator()() {
		Nid t1;
		Nid t2;

		if (!parse(value_<Nid>{t1}, value_<Nid>{t2}, noact)) {
			parse.syntax_error("'%query' requires two node ids");
			return;
		}
		if (!parse(';', noact)) {
			parse.warning("Expected ';'");
		}
		Nid r = dharc::query(t1, t2);
		std::cout << "  " << r << std::endl;
	}
};

struct command_partners {
	Context &parse;

	void operator()() {
		Nid t1;

		if (!parse(value_<Nid>{t1}, noact)) {
			parse.syntax_error("'%partners' needs a node id");
		}
		if (!parse(';', noact)) {
			parse.warning("Expected ';'");
		}
		auto part = dharc::partners(t1);
		for (auto i : part) {
			cout << "  - " << i << std::endl;
		}
	}
};

void parse_statement(Nid &cur, Context &parse) {
	Nid n;

	if (!(parse(';', noact)
	|| parse(value_<Nid>{n}, '=', [&]() {
		Nid r;

		if (!(parse(value_<Nid>{r}, [&]() {
			dharc::define(cur, n, r);
			parse_statement(cur, parse);
		}))) {
			parse.syntax_error("'=' must be followed by a node id");
		}
	})
	|| parse(value_<Nid>{n}, [&]() {
		if ((cur == dharc::null_n) || (n == dharc::null_n)) {
			parse.info("querying a 'null' node");
		}
		cur = dharc::query(cur, n);
		parse_statement(cur, parse);
	}))) {
		parse.syntax_error("Expected a node id");
	}
}
};  // namespace

Nid dharc::Script::operator()(const vector<Nid> &params) {
	Nid cur = null_n;

	while (!ctx.eof()) {
		if (!(ctx('%', [&]() {
			if (!(
				ctx(word_{"query"}, command_query{ctx})
				|| ctx(word_{"partners"}, command_partners{ctx})))
			{
				ctx.syntax_error("Unrecognised command");
			}
		})
		|| ctx(value_<Nid>{cur}, [&]() {
			parse_statement(cur, ctx);
		}))) {
			ctx.syntax_error("Invalid statement");
		}

		if (!ctx) {
			ctx.skip_line();
		}
		ctx.print_messages(source);
	}
	return cur;
}

