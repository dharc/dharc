/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"

#include "dharc/rpc_packer.hpp"

#include <vector>
#include <list>
#include <sstream>

using namespace dharc;
using std::vector;
using std::list;

const lest::test specification[] = {

CASE( "Pack numbers" ) {
	std::stringstream ss;
	rpc::Packer<int>::pack(ss, 3456);
	EXPECT( ss.str() == "3456" );

	ss.str("");
	rpc::Packer<float>::pack(ss, 34.56);
	EXPECT( ss.str() == "34.56" );
},

CASE( "Unpack numbers" ) {
	std::stringstream ss;
	ss.str("45667");
	EXPECT( rpc::Packer<int>::unpack(ss) == 45667 );
},

CASE( "Unpack numbers (fail)" ) {
	std::stringstream ss;
	ss.str("@445");
	EXPECT( rpc::Packer<int>::unpack(ss) == 0 );
},

CASE( "Pack and unpack a vector of numbers" ) {
	vector<int> vec = {
		45, 46, 47, 48
	};
	std::stringstream ss;
	rpc::Packer<vector<int>>::pack(ss, vec);
	EXPECT( ss.str() == "[45,46,47,48]" );
	vector<int> res;
	res = rpc::Packer<vector<int>>::unpack(ss);
	EXPECT( res[0] == 45 );
	EXPECT( res[3] == 48 );
},

CASE( "Pack and unpack a vector of vectors" ) {
	vector<vector<int>> vec = {
		{ 45, 46, 47, 48 },
		{ 23, 24, 25, 26 },
		{ 34, 35, 36, 37 }
	};
	std::stringstream ss;
	rpc::Packer<vector<vector<int>>>::pack(ss, vec);
	EXPECT( ss.str() == "[[45,46,47,48],[23,24,25,26],[34,35,36,37]]" );
	vector<vector<int>> res;
	res = rpc::Packer<vector<vector<int>>>::unpack(ss);
	EXPECT( res[0][0] == 45 );
	EXPECT( res[2][3] == 37 );
},

CASE( "Pack and unpack a list" ) {
	list<int> lst = { 56, 57, 58 };
	std::stringstream ss;
	rpc::Packer<list<int>>::pack(ss, lst);
	EXPECT( ss.str() == "[56,57,58]" );
	list<int> res;
	res = rpc::Packer<list<int>>::unpack(ss);
	EXPECT( res.front() == 56 );
	EXPECT( res.back() == 58 );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}

