#include "lest.hpp"

#include "dharc/node.hpp"

#include <sstream>

using namespace dharc;

using lest::approx;

const lest::test specification[] = {

CASE( "Integer literal" ) {
	Node n1 = 123_n;
	EXPECT( n1.t == Node::Type::integer );
	EXPECT( n1.i == 123 );
},

CASE( "Double literal") {
	Node n1 = 23.67_n;
	EXPECT( n1.t == Node::Type::real );
	EXPECT( n1.d == approx(23.67) );
},

CASE( "Character literal") {
	Node n1 = 'f'_n;
	EXPECT( n1.t == Node::Type::character );
	EXPECT( n1.c == 'f' );
},

/* CASE( "Generating unique nodes ids" ) {
	Node n1 = Node::unique();
	Node n2 = Node::unique();
	EXPECT( n1.t == Node::Type::allocated );
	EXPECT( n2.t == Node::Type::allocated );
	EXPECT( n1.i != n2.i );
},*/ 

CASE( "Node equality" ) {
	EXPECT( 1234_n == 1234_n );
	EXPECT( !(99_n == 55_n) );
},

CASE( "Node inequality" ) {
	EXPECT( 98_n != 'c'_n );
	EXPECT( !('d'_n != 'd'_n) );
},

CASE( "Node less than" ) {
	EXPECT( 100_n < 101_n );
	EXPECT( true_n < 1_n );
	EXPECT( null_n < true_n );
	EXPECT( !(300_n < 300_n) );
},

CASE( "Output stream integer nodes" ) {
	std::stringstream str;

	str << 100_n;
	EXPECT( str.str() == "100" );
},

CASE( "Output stream special nodes" ) {
	std::stringstream str;
	str << true_n;
	EXPECT( str.str() == "true" );
	str.str("");
	str << false_n;
	EXPECT( str.str() == "false" );
	str.str("");
	str << null_n;
	EXPECT( str.str() == "null" );
},

CASE( "Output stream character nodes" ) {
	std::stringstream str;
	str << 'a'_n;
	EXPECT( str.str() == "'a'" );
},

CASE( "Integer node from string" ) {
	EXPECT( Node("100") == 100_n );
	EXPECT( Node("1:100") == 100_n );
},

CASE( "Float node from string" ) {
	EXPECT( Node("11.2") == 11.2_n );
},

CASE( "Character node from string" ) {
	EXPECT( Node("'b'") == Node('b') );
},

CASE( "Special node from string" ) {
	EXPECT( Node("true") == true_n );
	EXPECT( Node("false") == false_n );
	EXPECT( Node("null") == null_n );
},

CASE( "Node packer pack" ) {
	std::stringstream ss;
	rpc::Packer<Node>::pack(ss, true_n);
	EXPECT( ss.str() == "\"0:1\"" );
},

CASE( "Node packer unpack" ) {
	std::stringstream ss;
	ss.str("\"0:2\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == false_n );
	ss.str("\"1:99\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == 99_n );
},

CASE( "Node packer unpack missing quotes (fail)" ) {
	std::stringstream ss;
	ss.str("1:200\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
	ss.str("\"1:200");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
	ss.str("1:200");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
},

CASE( "Node packer unpack incorrect colon (fail)" ) {
	std::stringstream ss;
	ss.str("\"1200\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
	ss.str("\":1200\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
	ss.str("\"1200:\"");
	EXPECT( rpc::Packer<Node>::unpack(ss) == null_n );
}

};

int main(int argc, char *argv[])
{
	return lest::run(specification);
}
