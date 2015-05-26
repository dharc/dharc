/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"
#include "dharc/parse.hpp"

#include <string>

using namespace dharc;
using namespace dharc::parser;

void complex_parse(Context &parse) {
	if (!parse(word_{"var"}, [&]() {
		token_t type;
		std::string id;

		if (!(parse(token_<word_>{{"int"},type,1}, noact)
			|| parse(token_<word_>{{"float"},type,2}, noact)
			|| parse(token_<word_>{{"bool"},type,3}, noact)
			)) {
			parse.syntax_error("Expected type after 'var'");
		}
		if (!parse(id_{id}, noact)) {
			parse.syntax_error("Invalid identifier after type");
		}
		if (!(parse("=", noact) || parse(";", noact))) {
			parse.syntax_error("Unexpected item after identifier '"+id+"'");
		}
	})) {
		parse.syntax_error("Invalid statement");
	}
}

const lest::test specification[] = {

CASE(  "Parse comma separated ints" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("100, 200, 300");
	
	int vals[3];
	EXPECT( parse(
		value_<int>{vals[0]},
		", ",
		value_<int>{vals[1]},
		", ",
		value_<int>{vals[2]}, noact) );
	EXPECT( vals[0] == 100 );
	EXPECT( vals[1] == 200 );
	EXPECT( vals[2] == 300 );
},

CASE( "Attempt to parse ints (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("100, oops, 300");
	
	int vals[3];
	EXPECT( parse(
		value_<int>{vals[0]},
		", ",
		value_<int>{vals[1]},
		", ",
		value_<int>{vals[2]}, noact) == false );
},

CASE( "Multiple parses of same string" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("hello 55");
	int val;
	EXPECT( parse("hello", "world", noact) == false );
	EXPECT( parse("hello", value_<int>{val}, '[', noact) == false );
	EXPECT( parse("hello", value_<int>{val}, noact) );
},

CASE( "Remove all kinds of whitespace" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("  \thello\t  \t\nworld");
	EXPECT( parse("hello", "world", noact) );
},

CASE( "Correctly check for a keyword" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void hello");
	EXPECT( parse(word_{"void"}, noact) );
},

CASE( "Keyword at end of input" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void");
	EXPECT( parse(word_{"void"}, noact) );
},

CASE( "Keyword not found (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void");
	EXPECT( parse(word_{"bool"}, noact) == false );
},

CASE( "Keyword as prefix (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    voided");
	EXPECT( parse(word_{"void"}, noact) == false );
	ss.str("    voidEd");
	EXPECT( parse(word_{"void"}, noact) == false );
	ss.str("    void00");
	EXPECT( parse(word_{"void"}, noact) == false );
},

CASE( "Keyword followed by non-alphanumeric" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void[");
	EXPECT( parse(word_{"void"}, noact) );
},

CASE( "Use of custom lamda" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("  /**  ");
	EXPECT( parse([](auto &ctx) {
		std::string str;
		std::getline(ctx.stream, str, ' ');
		if (str == "/**") return true;
		return false;
	}, noact) );
},

CASE( "Parsing valid identifiers" ) {
	std::stringstream ss;
	Context parse(ss);
	std::string id;
	ss.str("helloid[");
	EXPECT( parse(id_{id}, "[", noact) );
	EXPECT( id == "helloid" );
	ss.str("hello40[");
	EXPECT( parse(id_{id}, "[", noact) );
	EXPECT( id == "hello40" );
	ss.str("_hello_id ");
	EXPECT( parse(id_{id}, noact) );
	EXPECT( id == "_hello_id" );
},

CASE( "Parsing invalid identifiers (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	std::string id;
	ss.str("7elloid[");
	EXPECT( parse(id_{id}, "[", noact) == false );
	ss.str("&elloid[");
	EXPECT( parse(id_{id}, "[", noact) == false );
	ss.str("7[");
	EXPECT( parse(id_{id}, "[", noact) == false );
},

CASE( "Checking for individual characters" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("@[]");
	EXPECT( parse('@','[',']', noact) );
	ss.str(":?");
	EXPECT( parse('?', noact) == false);
},

CASE( "Using lambda as actor" ) {
	std::stringstream ss;
	Context parse(ss);
	std::string id;
	int val;
	bool success = false;
	ss.str("x = 55");

	parse(id_{id}, '=', value_<int>{val}, [&]() {
		success = true;
		EXPECT( val == 55 );
		EXPECT( id == "x" );
	});
	EXPECT( success );

	success = false;
	ss.str("x & 55");
	parse(id_{id}, '=', value_<int>{val}, [&]() {
		success = true;
	});
	EXPECT( success == false );
},

CASE( "Complex test parser" ) {
	std::stringstream ss;
	Context parse(ss);

	ss.str("var int myvar = 534;");
	complex_parse(parse);
	EXPECT( parse.success() );

	parse.reset();
	ss.str("var in myvar = 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	parse.reset();
	ss.str("var float 0myvar = 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	parse.reset();
	ss.str("var float myvar : 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	//parse.print_messages("parse_test");
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

