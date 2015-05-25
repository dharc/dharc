/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"
#include "dharc/parse.hpp"

using namespace dharc;

const lest::test specification[] = {

CASE(  "Parse comma separated ints" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("100, 200, 300");
	
	int vals[3];
	EXPECT( parse(
		value_<int>{vals[0]},
		", ",
		value_<int>{vals[1]},
		", ",
		value_<int>{vals[2]}) );
	EXPECT( vals[0] == 100 );
	EXPECT( vals[1] == 200 );
	EXPECT( vals[2] == 300 );
},

CASE( "Attempt to parse ints (fail)" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("100, oops, 300");
	
	int vals[3];
	EXPECT( parse(
		value_<int>{vals[0]},
		", ",
		value_<int>{vals[1]},
		", ",
		value_<int>{vals[2]}) == false );
},

CASE( "Multiple parses of same string" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("hello 55");
	int val;
	EXPECT( parse("hello", "world") == false );
	EXPECT( parse("hello", value_<int>{val}) );
},

CASE( "Remove all kinds of whitespace" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("  \thello\t  \t\nworld");
	EXPECT( parse("hello", "world") );
},

CASE( "Correctly check for a keyword" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("    void hello");
	EXPECT( parse(word_{"void"}) );
},

CASE( "Keyword at end of input" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("    void");
	EXPECT( parse(word_{"void"}) );
},

CASE( "Keyword not found (fail)" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("    void");
	EXPECT( parse(word_{"bool"}) == false );
},

CASE( "Keyword as prefix (fail)" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("    voided");
	EXPECT( parse(word_{"void"}) == false );
	ss.str("    voidEd");
	EXPECT( parse(word_{"void"}) == false );
	ss.str("    void00");
	EXPECT( parse(word_{"void"}) == false );
},

CASE( "Keyword followed by non-alphanumeric" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("    void[");
	EXPECT( parse(word_{"void"}) );
},

CASE( "Use of custom lamda" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("  /**  ");
	EXPECT( parse([](auto &ctx) {
		std::string str;
		std::getline(ctx.stream, str, ' ');
		if (str == "/**") return true;
		return false;
	}) );
},

CASE( "Parsing valid identifiers" ) {
	std::stringstream ss;
	Parser parse(ss);
	std::string id;
	ss.str("helloid[");
	EXPECT( parse(id_{id}, "[") );
	EXPECT( id == "helloid" );
	ss.str("hello40[");
	EXPECT( parse(id_{id}, "[") );
	EXPECT( id == "hello40" );
	ss.str("_hello_id ");
	EXPECT( parse(id_{id}) );
	EXPECT( id == "_hello_id" );
},

CASE( "Parsing invalid identifiers (fail)" ) {
	std::stringstream ss;
	Parser parse(ss);
	std::string id;
	ss.str("7elloid[");
	EXPECT( parse(id_{id}, "[") == false );
	ss.str("&elloid[");
	EXPECT( parse(id_{id}, "[") == false );
	ss.str("7[");
	EXPECT( parse(id_{id}, "[") == false );
},

CASE( "Checking for individual characters" ) {
	std::stringstream ss;
	Parser parse(ss);
	ss.str("@[]");
	EXPECT( parse('@','[',']') );
	ss.str(":?");
	EXPECT( parse('?') == false);
},

CASE( "Using lambda as actor" ) {
	std::stringstream ss;
	Parser parse(ss);
	std::string id;
	int val;
	bool success = false;
	ss.str("x = 55");

	parse(id_{id}, '=', value_<int>{val}, [&](auto &ctx) {
		success = true;
		EXPECT( val == 55 );
		EXPECT( id == "x" );
		return true;
	});
	EXPECT( success );

	success = false;
	ss.str("x & 55");
	parse(id_{id}, '=', value_<int>{val}, [&](auto &ctx) {
		success = true;
		return true;
	});
	EXPECT( success == false );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

