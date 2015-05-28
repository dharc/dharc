/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"
#include "dharc/parse.hpp"

#include <string>

using namespace dharc;
using namespace dharc::parser;

void complex_parse(Context &parse) {
	if (!parse(word{"var"}, [&]() {
		token_t type;
		std::string id_tmp;

		if (!(parse(token<word>{{"int"},type,1}, noact)
			|| parse(token<word>{{"float"},type,2}, noact)
			|| parse(token<word>{{"bool"},type,3}, noact)
			)) {
			parse.syntaxError("Expected type after 'var'");
		}
		if (!parse(id{id_tmp}, noact)) {
			parse.syntaxError("Invalid identifier after type");
		}
		if (!(parse("=", noact) || parse(";", noact))) {
			parse.syntaxError("Unexpected item after identifier '"+id_tmp+"'");
		}
	})) {
		parse.syntaxError("Invalid statement");
	}
}

const lest::test specification[] = {

CASE(  "Parse comma separated ints" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("100, 200, 300");
	
	int vals[3];
	EXPECT( parse(
		value<int>{vals[0]},
		',',
		value<int>{vals[1]},
		',',
		value<int>{vals[2]}, noact) );
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
		value<int>{vals[0]},
		", ",
		value<int>{vals[1]},
		", ",
		value<int>{vals[2]}, noact) == false );
},

CASE( "Multiple parses of same string" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("hello 55");
	int val;
	EXPECT( parse("hello", "world", noact) == false );
	EXPECT( parse("hello", value<int>{val}, '[', noact) == false );
	EXPECT( parse("hello", value<int>{val}, noact) );
	EXPECT( val == 55 );
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
	EXPECT( parse(word{"void"}, noact) );
},

CASE( "Check for massive non-existing keyword (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("const int myvar = 45;");
	EXPECT( parse(word{"somemassivekeywordthatsurelycannotbearealthing"
						"becauseitwouldtakeforevertotype"}, noact) == false );
},

CASE( "Check for massive keyword" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("somemassivekeywordthatsurelycannotbearealthing"
						"becauseitwouldtakeforevertotype");
	EXPECT( parse(word{"somemassivekeywordthatsurelycannotbearealthing"
						"becauseitwouldtakeforevertotype"}, noact) );
},

CASE( "Keyword at end of input" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    const");
	EXPECT( parse(word{"const"}, noact) );
},

CASE( "Keyword not found (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void");
	EXPECT( parse(word{"bool"}, noact) == false );
},

CASE( "Keyword as prefix (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    voided");
	EXPECT( parse(word{"void"}, noact) == false );
	ss.str("    voidEd");
	EXPECT( parse(word{"void"}, noact) == false );
	ss.str("    void00");
	EXPECT( parse(word{"void"}, noact) == false );
},

CASE( "Keyword followed by non-alphanumeric" ) {
	std::stringstream ss;
	Context parse(ss);
	ss.str("    void[");
	EXPECT( parse(word{"void"}, noact) );
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
	std::string id_tmp;
	ss.str("helloid[");
	EXPECT( parse(id{id_tmp}, "[", noact) );
	EXPECT( id_tmp == "helloid" );
	ss.str("hello40[");
	EXPECT( parse(id{id_tmp}, "[", noact) );
	EXPECT( id_tmp == "hello40" );
	ss.str("_hello_id ");
	EXPECT( parse(id{id_tmp}, noact) );
	EXPECT( id_tmp == "_hello_id" );
},

CASE( "Parsing invalid identifiers (fail)" ) {
	std::stringstream ss;
	Context parse(ss);
	std::string id_tmp;
	ss.str("7elloid[");
	EXPECT( parse(id{id_tmp}, "[", noact) == false );
	ss.str("&elloid[");
	EXPECT( parse(id{id_tmp}, "[", noact) == false );
	ss.str("7[");
	EXPECT( parse(id{id_tmp}, "[", noact) == false );
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
	std::string id_tmp;
	int val;
	bool success = false;
	ss.str("x = 55");

	parse(id{id_tmp}, '=', value<int>{val}, [&]() {
		success = true;
		EXPECT( val == 55 );
		EXPECT( id_tmp == "x" );
	});
	EXPECT( success );

	success = false;
	ss.str("x & 55");
	parse(id{id_tmp}, '=', value<int>{val}, [&]() {
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

	parse.printMessages("parse_test");

	parse.reset();
	ss.str("var in myvar = 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	parse.printMessages("parse_test");

	parse.reset();
	ss.str("var float 0myvar = 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	parse.printMessages("parse_test");

	parse.reset();
	ss.str("var float myvar : 534;");
	complex_parse(parse);
	EXPECT( parse.failed() );

	parse.printMessages("parse_test");
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

