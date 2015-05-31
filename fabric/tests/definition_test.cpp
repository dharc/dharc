#include "lest.hpp"

#include <string>

#include "dharc/node.hpp"
#include "dharc/definition.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;
using namespace std;

/* ==== MOCKS ==== */

Fabric dharc::fabric;

Fabric::Fabric() {
}

Fabric::~Fabric() {
}


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
	switch(n.t) {
	case Node::Type::special    :
		switch(n.i) {
		case kNullNode   : os << "null";   break;
		case kTrueNode   : os << "true";   break;
		case kFalseNode  : os << "false";  break;
		}
		break;
	case Node::Type::integer    : os << n.i;                 break;
	case Node::Type::real       : os << n.d;                 break;
	case Node::Type::character  : os << '\'' << n.c << '\''; break;
	default : os << static_cast<int>(n.t) << ':' << n.i;
	}
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

	EXPECT( ss.str() == "{(978 888) (44 45)}" );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

