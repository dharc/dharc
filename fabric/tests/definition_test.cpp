#include "lest.hpp"

#include <string>

#include "dharc/node.hpp"
#include "dharc/definition.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;
using namespace std;

/* ==== MOCKS ==== */



Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	return p[0];
}

vector<Node> Fabric::paths(const vector<vector<Node>> &p, const Harc *dep) {
	vector<Node> result(p.size());
	int ix = 0;
	for (auto i : p) {
		result[ix++] = i[0];
	}
	return result;
}

ostream &dharc::operator<<(ostream &os, const Node &n) {
	os << '<' << n.value << '>';
	return os;
}

/* ==== END MOCKS ==== */

const lest::test specification[] = {

CASE( "Generates expected paths on evaluation" ) {
	Definition *def = Definition::makeDefinition(
			{{978_n, 888_n}, {44_n, 45_n}});
	EXPECT( def->evaluate(nullptr) == 978_n );
},

CASE( "Writing a definition to stream" ) {
	std::stringstream ss;
	Definition *def = Definition::makeDefinition(
			{{978_n, 888_n}, {44_n, 45_n}});

	ss << *def;

	EXPECT( ss.str() == "{(<978> <888>) (<44> <45>)}" );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}

