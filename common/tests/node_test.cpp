#include "lest.hpp"

#include "dharc/node.hpp"

#include <sstream>
#include <string>

using namespace dharc;
using std::string;

using lest::approx;

const lest::test specification[] = {

CASE( "Node literal" ) {
	Node n1 = 123_n;
	EXPECT( n1.value == 123U );
},

CASE( "Node equality" ) {
	EXPECT( 1234_n == 1234_n );
	EXPECT( !(99_n == 55_n) );
},

CASE( "Node inequality" ) {
	EXPECT( 98_n != 90_n );
	EXPECT( !(90_n != 90_n) );
},

CASE( "Node less than" ) {
	EXPECT( 100_n < 101_n );
	EXPECT( !(100_n < 100_n) );
	EXPECT( !(101_n < 100_n) );
},

CASE( "Output stream nodes" ) {
	std::stringstream str;

	str << 100_n;
	EXPECT( str.str() == "<100>" );
},

CASE( "Node from string" ) {
	EXPECT( Node("<100>") == 100_n );
},

CASE( "Node to string" ) {
	EXPECT( (string)100_n == "<100>" );
}

};

int main(int argc, char *argv[])
{
	return lest::run(specification, argc, argv);
}
