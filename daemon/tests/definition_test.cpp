#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/definition.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

/* ==== MOCKS ==== */

Fabric &fdsb::fabric = Fabric::singleton();

Fabric::Fabric() {
}

Fabric::~Fabric() {
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

Nid dummy_path;

Nid Fabric::path(const vector<vector<Nid>> &p, Harc *dep) {
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

std::ostream &fdsb::operator<<(std::ostream &os, const Nid &n) {
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

void test_definition_eval() {
	Definition *def = Definition::from_path({{}});
	dummy_path = 978_n;
	CHECK(def->evaluate(nullptr) == 978_n);
	dummy_path = 956_n;
	CHECK(def->evaluate(nullptr) == 978_n);
	def->mark();
	CHECK(def->evaluate(nullptr) == 956_n);
	DONE;
}

void test_definition_strings() {
	Definition *def;

	def = Definition::from_string("{([100] [200])}");
	CHECK(def != nullptr);
	CHECK(def->to_path()[0][0] == 100_n);
	CHECK(def->to_string() == "{([100] [200])}");
	delete def;

	def = Definition::from_string("		{100}");
	CHECK(def != nullptr);
	CHECK(def->to_path()[0][0] == 100_n);
	CHECK(def->to_string() == "{([100])}");
	delete def;

	def = Definition::from_string("{([100] [200])  ( [300]   [400])}");
	CHECK(def != nullptr);
	CHECK(def->to_path()[1][1] == 400_n);
	CHECK(def->to_string() == "{([100] [200])([300] [400])}");
	delete def;

	def = Definition::from_string("");
	CHECK(def == nullptr);
	def = Definition::from_string("([100])");
	CHECK(def == nullptr);
	def = Definition::from_string("{([100])");
	CHECK(def == nullptr);

	DONE;
}

int main(int argc, char *argv[]) {
	test(test_definition_eval);
	test(test_definition_strings);
	return test_fail_count();
}

