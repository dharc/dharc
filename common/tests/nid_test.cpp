#include "lest.hpp"

#include "dharc/nid.hpp"

#include <sstream>

using namespace dharc;

using lest::approx;

const lest::test specification[] = {

CASE( "Integer literal" ) {
	Nid n1 = 123_n;
	EXPECT( n1.t == Nid::Type::integer );
	EXPECT( n1.i == static_cast<unsigned>(123) );
},

CASE( "Double literal") {
	Nid n1 = 23.67_n;
	EXPECT( n1.t == Nid::Type::real );
	EXPECT( n1.d == approx(23.67) );
},

CASE( "Character literal") {
	Nid n1 = 'f'_n;
	EXPECT( n1.t == Nid::Type::character );
	EXPECT( n1.c == 'f' );
},

CASE( "Generating unique nodes ids" ) {
	Nid n1 = Nid::unique();
	Nid n2 = Nid::unique();
	EXPECT( n1.t == Nid::Type::allocated );
	EXPECT( n2.t == Nid::Type::allocated );
	EXPECT( n1.i != n2.i );
},

CASE( "Nid equality" ) {
	EXPECT( 1234_n == 1234_n );
	EXPECT( !(99_n == 55_n) );
},

CASE( "Nid inequality" ) {
	EXPECT( 98_n != 'c'_n );
	EXPECT( !('d'_n != 'd'_n) );
},

CASE( "Nid less than" ) {
	EXPECT( 100_n < 101_n );
	EXPECT( true_n < 1_n );
	EXPECT( null_n < true_n );
	EXPECT( !(300_n < 300_n) );
},

CASE( "Output stream integer nodes" ) {
	std::stringstream str;

	str << 100_n;
	EXPECT( str.str() == "[100]" );
},

CASE( "Output stream special nodes" ) {
	std::stringstream str;
	str << true_n;
	EXPECT( str.str() == "[true]" );
	str.str("");
	str << false_n;
	EXPECT( str.str() == "[false]" );
	str.str("");
	str << null_n;
	EXPECT( str.str() == "[null]" );
},

CASE( "Output stream character nodes" ) {
	std::stringstream str;
	str << 'a'_n;
	EXPECT( str.str() == "['a']" );
},

CASE( "Integer node from string" ) {
	EXPECT( Nid::from_string("100") == 100_n );
	EXPECT( Nid::from_string("[100]") == 100_n );
	EXPECT( Nid::from_string("1:100") == 100_n );
	EXPECT( Nid::from_string("[1:100]") == 100_n );
},

CASE( "Float node from string" ) {
	EXPECT( Nid::from_string("11.2") == 11.2_n );
	EXPECT( Nid::from_string("[11.2]") == 11.2_n );
},

CASE( "Character node from string" ) {
	EXPECT( Nid::from_string("'b'") == 'b'_n );
	EXPECT( Nid::from_string("['b']") == 'b'_n );
},

CASE( "Special node from string" ) {
	EXPECT( Nid::from_string("true") == true_n );
	EXPECT( Nid::from_string("[true]") == true_n );
	EXPECT( Nid::from_string("false") == false_n );
	EXPECT( Nid::from_string("[false]") == false_n );
	EXPECT( Nid::from_string("null") == null_n );
	EXPECT( Nid::from_string("[null]") == null_n );
},

CASE( "Nid packer pack" ) {
	std::stringstream ss;
	rpc::Packer<Nid>::pack(ss, true_n);
	EXPECT( ss.str() == "\"0:1\"" );
},

CASE( "Nid packer unpack" ) {
	std::stringstream ss;
	ss.str("\"0:2\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == false_n );
	ss.str("\"1:99\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == 99_n );
},

CASE( "Nid packer unpack missing quotes (fail)" ) {
	std::stringstream ss;
	ss.str("1:200\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
	ss.str("\"1:200");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
	ss.str("1:200");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
},

CASE( "Nid packer unpack incorrect colon (fail)" ) {
	std::stringstream ss;
	ss.str("\"1200\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
	ss.str("\":1200\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
	ss.str("\"1200:\"");
	EXPECT( rpc::Packer<Nid>::unpack(ss) == null_n );
}

};

int main(int argc, char *argv[])
{
	return lest::run(specification);
}
