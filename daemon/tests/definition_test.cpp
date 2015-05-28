#include "lest.hpp"

#include <string>

#include "dharc/node.hpp"
#include "dharc/definition.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;
using namespace std;

/* ==== MOCKS ==== */

Fabric &dharc::fabric = Fabric::singleton();

Fabric::Fabric() {
}

Fabric::~Fabric() {
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

Node dummy_path;

Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	return dummy_path;
}

vector<Node> Fabric::paths(const vector<vector<Node>> &p, const Harc *dep) {
	vector<Node> result(p.size());
	return result;
}

std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
	switch(n.t) {
	case Node::Type::integer:
		os << '[' << n.i << ']';
		break;
	default:
		os << '[' << static_cast<int>(n.t) << ':' << n.i << ']';
		break;
	}
	return os;
}

/* ==== END MOCKS ==== */

const lest::test specification[] = {

CASE( "Evaluates only when out-of-date" ) {
	Definition *def = Definition::makeDefinition({{}});
	dummy_path = 978_n;
	EXPECT( def->evaluate(nullptr) == 978_n );
	dummy_path = 956_n;
	EXPECT( def->evaluate(nullptr) == 956_n );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

