/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"
#include "dharc/rpc_common.hpp"
#include "dharc/nid.hpp"

#include <string>
#include <vector>

using namespace dharc;
using std::vector;

/* ==== MOCKS =============================================================== */

std::string dummy_result;
std::string gen_string;

std::string dharc::rpc::intern::send(const std::string &s) {
	gen_string = s;
	return dummy_result;
}

/* ========================================================================== */

const lest::test specification[] = {

CASE( "Version no arguments command send" ) {
	dummy_result = "66";
	EXPECT( rpc::send<rpc::Command::version>() == 66 );
	EXPECT( gen_string == "{\"c\": 1, \"args\": []}" );
},

CASE( "Query two Nid argument command send" ) {
	dummy_result = "\"0:0\"";
	EXPECT( rpc::send<rpc::Command::query>(55_n, 66_n) == null_n );
	EXPECT( gen_string == "{\"c\": 2, \"args\": [\"1:55\",\"1:66\"]}" );
},

CASE( "Define with vector argument" ) {
	dummy_result = "1";
	vector<vector<Nid>> def = {{100_n, 200_n, 300_n}};
	EXPECT( rpc::send<rpc::Command::define>(12_n, 13_n, def) == true );
	EXPECT( gen_string == "{\"c\": 4, \"args\": [\"1:12\",\"1:13\",[[\"1:100\",\"1:200\",\"1:300\"]]]}" );
},

/*
CASE( "Invalid command (static fail)" ) {
	rpc::send<static_cast<rpc::Command>(5000)>();
},*/

/*
CASE( "Incorrect arguments (static fail)" ) {
	rpc::send<rpc::Command::partners>(44_n, 55_n);
},
*/

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

