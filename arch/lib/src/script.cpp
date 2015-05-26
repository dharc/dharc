/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/script.hpp"

#include <vector>
#include <iostream>
#include <string>

#include "dharc/nid.hpp"
#include "dharc/parse.hpp"
#include "dharc/arch.hpp"

using dharc::Nid;
using dharc::parser::Context;
using std::vector;
using std::string;
using std::cout;
using dharc::parser::word_;
using dharc::parser::value_;
using dharc::parser::noact;

dharc::Script::Script(std::istream &is, const char *src)
: ctx(is), source(src), params(nullptr) {
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
};  // namespace

bool dharc::Script::parse_node(Context &ctx, Nid &value) {
	return (ctx(value_<Nid>{value}, noact)
	|| ctx(word_{"<new>"}, [&]() { value = dharc::unique(); })
	|| ctx('$', [&]() {
		int id = 0;
		if (!ctx(value_<int>{id}, noact)) {
			ctx.syntax_error("'$' must be followed by a parameter number");
		}
		if (id >= static_cast<int>(params->size())) {
			ctx.runtime_error(string("parameter '$")
				+ std::to_string(id)
				+ "' not found");
			value = dharc::null_n;
		} else {
			value = (*params)[id];
		}
	}));
}

void dharc::Script::parse_statement(Nid &cur, Context &parse) {
	Nid n;

	if (parse(';', noact)) return;
	if (parse_node(parse, n)) {
		if (parse('=', noact)) {
			Nid r;

			if (parse_node(parse, r)) {
				dharc::define(cur, n, r);
				parse_statement(cur, parse);
			} else {
				parse.syntax_error("'=' must be followed by a node id");
			}
		} else {
			if ((cur == dharc::null_n) || (n == dharc::null_n)) {
				parse.info("querying a 'null' node");
			}
			cur = dharc::query(cur, n);
			parse_statement(cur, parse);
		}
	} else {
		if (parse.eof()) {
			parse.warning("expected a node id or ';'");
		} else {
			parse.syntax_error("expected a node id");
		}
	}
}

Nid dharc::Script::operator()(const vector<Nid> &p) {
	Nid cur = null_n;
	params = &p;

	while (!ctx.eof()) {
		if (ctx('%', noact)) {
			if (!(
				ctx(word_{"query"}, command_query{ctx})
				|| ctx(word_{"partners"}, command_partners{ctx})))
			{
				ctx.syntax_error("Unrecognised command");
			}
		} else if (parse_node(ctx, cur)) {
			parse_statement(cur, ctx);
		} else {
			ctx.syntax_error("Invalid statement");
		}

		if (!ctx) {
			ctx.skip_line();
		}
		ctx.print_messages(source);
	}
	return cur;
}

