/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/script.hpp"

#include <vector>
#include <iostream>
#include <string>

#include "dharc/node.hpp"
#include "dharc/parse.hpp"
#include "dharc/arch.hpp"
#include "dharc/labels.hpp"

using dharc::Node;
using dharc::parser::Context;
using std::vector;
using std::string;
using std::cout;

using dharc::parser::word;
using dharc::parser::value;
using dharc::parser::id;
using dharc::parser::noact;

dharc::arch::Script::Script(std::istream &is, const char *src)
: ctx_(is), source_(src), params_(nullptr), printmessages_(true) {
}

namespace {
struct command_query {
	Context &parse;

	void operator()() {
		Node t1;
		Node t2;

		if (!parse(value<Node>{t1}, value<Node>{t2}, noact)) {
			parse.syntaxError("'%query' requires two node ids");
			return;
		}
		if (!parse(';', noact)) {
			parse.warning("Expected ';'");
		}
		Node r = dharc::query(t1, t2);
		std::cout << "  " << r << std::endl;
	}
};

struct command_partners {
	Context &parse;

	void operator()() {
		Node t1;

		if (!parse(value<Node>{t1}, noact)) {
			parse.syntaxError("'%partners' needs a node id");
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

bool dharc::arch::Script::parseNode(Node &val) {
	string labelstr;

	if (ctx_(value<Node>{val}, noact)) {
		return true;
	} else if (ctx_(id{labelstr}, noact)) {
		val = dharc::labels.getNode(labelstr);
		if (val.i == -1) ctx_.runtimeError("Label does not exist");
		return (val.i != -1);
	} else if (ctx_('$', noact)) {
		int id = 0;
		if (!ctx_(value<int>{id}, noact)) {
			ctx_.syntaxError("'$' must be followed by a parameter number");
			return false;
		}
		if (id >= static_cast<int>(params_->size())) {
			ctx_.runtimeError(string("parameter '$")
				+ std::to_string(id)
				+ "' not found");
			val = dharc::null_n;
		} else {
			val = (*params_)[id];
		}
		return true;
	}
	return false;
}

void dharc::arch::Script::parseDefinition(vector<vector<Node>> &def) {
	Node temp;

	while (true) {
		if (ctx_.eof()) {
			ctx_.warning("Missing '}' at end of definition");
			return;
		}
		if (ctx_('}', noact)) return;
		if (ctx_(';', noact)) {
			ctx_.warning("Missing '}' in definition");
			return;
		}

		if (ctx_('(', noact)) {
			def.push_back({});

			while (true) {
				if (ctx_.eof()) {
					ctx_.warning("Missing ')' in definition");
					return;
				}
				if (ctx_(')', noact)) break;
				if (ctx_(';', noact)) {
					ctx_.warning("Missing ')' in definition");
					return;
				}

				if (parseNode(temp)) {
					def.back().push_back(temp);

				} else if (ctx_('(', noact)) {
					ctx_.syntaxError("Un-normalised definitions not supported");
					return;

				} else {
					ctx_.syntaxError("Need a node or ')' in path");
					break;
				}
			}

		} else if (parseNode(temp)) {
			def.push_back({temp});

		} else {
			ctx_.syntaxError("Need a node or '(' in definition");
			return;
		}
	}
}

void dharc::arch::Script::parseStatement(Node &cur) {
	Node n;

	if (ctx_(';', noact)) return;

	if (ctx_(word{"<typeint>"}, noact)) {
		cur = Node(static_cast<int>(cur.t));
		parseStatement(cur);

	} else if (ctx_(word{"<int>"}, noact)) {
		cur = Node(static_cast<int>(cur.i));
		parseStatement(cur);

	} else if (parseNode(n)) {
		if (ctx_('=', noact)) {
			Node r;
			hasresult_ = false;

			if (ctx_('{', noact)) {
				vector<vector<Node>> def;
				parseDefinition(def);
				dharc::define(cur, n, def);

			} else if (parseNode(r)) {
				dharc::define(cur, n, r);
				parseStatement(cur);

			} else {
				ctx_.syntaxError("'=' must be followed by a node or '{'");
			}

		} else {
			if ((cur == dharc::null_n) || (n == dharc::null_n)) {
				ctx_.information("querying a 'null' node");
			}
			cur = dharc::query(cur, n);
			hasresult_ = true;
			parseStatement(cur);
		}

	} else {
		if (ctx_.eof()) {
			ctx_.warning("expected a node id or ';'");

		} else {
			ctx_.syntaxError("expected a node id");
		}
	}
}

void dharc::arch::Script::commandDefine() {
	string labelstr;
	int labelid;

	if (ctx_(id{labelstr}, value<int>{labelid}, noact)) {
		if (!dharc::labels.set(labelid, labelstr)) {
			ctx_.warning("Label already exists");
		}
	} else {
		ctx_.syntaxError("Try: %define <labelstring> <int>");
	}
}

void dharc::arch::Script::commandLabel() {
	string labelstr;
	int labelid;

	if (ctx_(value<int>{labelid}, noact)) {
		cout << "  " << labelid << " = " << dharc::labels.get(labelid) << std::endl;
	} else if (ctx_(id{labelstr}, noact)) {
		cout << "  " << labelstr << " = ";
		cout << dharc::labels.get(labelstr) << std::endl;
	} else {
		ctx_.syntaxError("Try: %define <labelstring> <int>");
	}
}

Node dharc::arch::Script::operator()(const vector<Node> &p) {
	Node cur = null_n;
	params_ = &p;
	hasresult_ = false;

	while (!ctx_.eof()) {
		if (ctx_('%', noact)) {
			if (ctx_(word{"define"}, noact)) {
				commandDefine();
			} else if (ctx_(word{"label"}, noact)) {
				commandLabel();
			} else {
				ctx_.syntaxError("Unrecognised command");
			}
		} else if (parseNode(cur)) {
			hasresult_ = true;
			parseStatement(cur);
		} else if (ctx_(word{"<typeint>"}, noact)) {
			ctx_.syntaxError("node cast '<typeint>' must follow node");
		} else if (ctx_(';', noact)) {
			// Nothing
		} else {
			ctx_.syntaxError("Invalid statement");
		}

		if (!ctx_) {
			ctx_.skipLine();
		}
		if (printmessages_) ctx_.printMessages(source_);
	}
	return cur;
}

