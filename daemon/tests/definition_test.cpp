#include "lest.hpp"

#include "dharc/nid.hpp"
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

Nid dummy_path;

Nid Fabric::path(const vector<vector<Nid>> &p, const Harc *dep) {
	return dummy_path;
}

Nid Nid::from_string(const std::string &str) {
	Nid r = null_n;

	if (str.size() > 0) {
		if (str.at(0) >= '0' && str.at(0) <= '9') {
			r.t = Nid::Type::integer;
			r.i = stoll(str);
		} else if (str.at(0) == '[') {
			string str2 = str.substr(1, str.size() - 2);
			return from_string(str2);
		}
	}

	return r;
}

std::ostream &dharc::operator<<(std::ostream &os, const Nid &n) {
	switch(n.t) {
	case Nid::Type::integer:
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
	Definition *def = Definition::from_path({{}});
	dummy_path = 978_n;
	EXPECT( def->evaluate(nullptr) == 978_n );
	dummy_path = 956_n;
	EXPECT( def->evaluate(nullptr) == 978_n );
	def->mark();
	EXPECT( def->evaluate(nullptr) == 956_n );
},

CASE( "Parse single two element path from string" ) {
	Definition *def;
	def = Definition::from_string("{([100] [200])}");

	EXPECT( def != nullptr );
	EXPECT( def->to_path()[0][0] == 100_n );
	EXPECT( def->to_string() == "{([100] [200])}" );

	delete def;
},

CASE( "Parse single unnested node from string" ) {
	Definition *def;
	def = Definition::from_string("		{100}");

	EXPECT( def != nullptr );
	EXPECT( def->to_path()[0][0] == 100_n );
	EXPECT( def->to_string() == "{([100])}" );

	delete def;
},

CASE( "Parse two two node paths with excess white space" ) {
	Definition *def;
	def = Definition::from_string("{([100] [200])  ( [300]   [400])}");

	EXPECT( def != nullptr );
	EXPECT( def->to_path()[1][1] == 400_n );
	EXPECT( def->to_string() == "{([100] [200])([300] [400])}" );

	delete def;
},

CASE( "Empty string (fail)" ) {
	Definition *def = Definition::from_string("");
	EXPECT( def == nullptr );
},

CASE( "Missing braces (fail)" ) {
	Definition *def = Definition::from_string("([100])");
	EXPECT( def == nullptr );
},

CASE( "Missing close brace (fail)" ) {
	Definition *def = Definition::from_string("{([100])");
	EXPECT( def == nullptr );
},

CASE( "Missing open bracket (fail)" ) {
	Definition *def = Definition::from_string("{[100])}");
	EXPECT( def == nullptr );
},

CASE( "Missing close bracket (fail)" ) {
	Definition *def = Definition::from_string("{([100]}");
	EXPECT( def == nullptr );
}

};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

