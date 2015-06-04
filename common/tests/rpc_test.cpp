/*
 * Copyright 2015 Nicolas Pope
 */

#include "lest.hpp"

// HACK
#include "zmq.hpp"
#define socket_t dummy
namespace zmq {
	typedef int dummy;
};  // namespace zmq

#include "dharc/rpc.hpp"
#include "dharc/node.hpp"

#include <string>
#include <vector>
#include <list>

using namespace dharc;
using std::vector;
using std::list;

/* ==== MOCKS =============================================================== */

std::string dummy_result;
std::string gen_string;


dharc::Rpc::Rpc(const char *addr, int port) {

}

dharc::Rpc::~Rpc() {

}

std::string dharc::Rpc::send(const std::string &s) {
	gen_string = s;
	return dummy_result;
}

std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
	os << '<' << static_cast<int>(n.value) << '>';
	return os;
}


/* ========================================================================== */


class TestRpc : public dharc::Rpc {
	public:
	TestRpc() : Rpc("", 0) {}
	~TestRpc() {}

	int version() {
		return send<Command::version>();
	}

	Node query(const Node &n1, const Node &n2) {
		return send<Command::query>(n1, n2);
	}

	bool define(const Node &n1,
				const Node &n2,
				const vector<Node> &def) {
		return send<Command::define>(n1, n2, def);
	}

	vector<Tail> partners(const Node &n, const int &count) {
		return send<Command::partners>(n, count);
	}
};


/* ===========================================================================*/

const lest::test specification[] = {

CASE( "Version no arguments command send" ) {
	TestRpc testrpc;
	dummy_result = "66";
	EXPECT( testrpc.version() == 66 );
	EXPECT( gen_string == "{\"c\": 1, \"args\": []}" );
},

CASE( "Query two Node argument command send" ) {
	TestRpc testrpc;
	dummy_result = "\"0:0\"";
	EXPECT( testrpc.query(55_n, 66_n) == null_n );
	EXPECT( gen_string == "{\"c\": 2, \"args\": [\"55\",\"66\"]}" );
},

CASE( "Define with vector argument" ) {
	TestRpc testrpc;
	dummy_result = "1";
	vector<Node> def = {100_n, 200_n, 300_n};
	EXPECT( testrpc.define(12_n, 13_n, def) == true );
	EXPECT( gen_string ==
		"{\"c\": 4, \"args\": [\"12\",\"13\",[\"100\",\"200\",\"300\"]]}" );
},

CASE( "Returning a vector" ) {
	TestRpc testrpc;
	dummy_result = "[[\"133\",\"344\"]]";
	int count = 10;
	vector<Tail> result = testrpc.partners(10_n, count);
	EXPECT( result.front().at(0) == 133_n );
	EXPECT( result.front().at(result.front().size() - 1) == 344_n );
}

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
	return lest::run(specification, argc, argv);
}

