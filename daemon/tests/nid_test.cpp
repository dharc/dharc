#include "dharc/test.hpp"
#include "dharc/nid.hpp"

#include <sstream>

using namespace dharc;

void test_nid_ilit()
{
	Nid n1 = 123_n;
	CHECK(n1.t == Nid::Type::integer);
	CHECK(n1.i == 123);
	DONE;
}

void test_nid_dlit()
{
	Nid n1 = 23.67_n;
	CHECK(n1.t == Nid::Type::real);
	CHECK(n1.d > 23.66 && n1.d < 23.68);
	DONE;
}

void test_nid_clit()
{
	Nid n1 = 'f'_n;
	CHECK(n1.t == Nid::Type::character);
	CHECK(n1.c == 'f');
	DONE;
}

void test_nid_unique()
{
	Nid n1 = Nid::unique();
	Nid n2 = Nid::unique();
	CHECK(n1.t == Nid::Type::allocated);
	CHECK(n1.i == 0);
	CHECK(n2.t == Nid::Type::allocated);
	CHECK(n2.i == 1);
	DONE;
}

void test_nid_eqne()
{
	Nid n1 = 1234_n;
	Nid n2 = 99_n;
	CHECK(!(n1 == n2));
	CHECK(n1 != n2);
	n2 = 1234_n;
	CHECK(n1 == n2);
	CHECK(!(n1 != n2));
	DONE;
}

void test_nid_less() {
	Nid n1 = 100_n;
	Nid n2 = 102_n;
	
	CHECK(n1 < n2);
	CHECK(!(n2 < n1));
	n1 = true_n;
	CHECK(n1 < n2);
	n1 = 102_n;
	CHECK(!(n1 <n2));
	DONE;
}

void test_nid_ostream() {
	std::stringstream str;
	str << 100_n;
	CHECK(str.str() == "[100]");
	str.str("");
	str << true_n;
	CHECK(str.str() == "[true]");
	DONE;
}

void test_nid_fromstr() {
	CHECK(Nid::from_string("100") == 100_n);
	CHECK(Nid::from_string("[100]") == 100_n);
	CHECK(Nid::from_string("'a'") == 'a'_n);
	CHECK(Nid::from_string("true") == true_n);
	CHECK(Nid::from_string("null") == null_n);
	CHECK(Nid::from_string("[false]") == false_n);
	CHECK(Nid::from_string("[true") == null_n);
	CHECK(Nid::from_string("11.2") == 11.2_n);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_nid_ilit);
	test(test_nid_dlit);
	test(test_nid_clit);
	test(test_nid_unique);
	test(test_nid_eqne);
	test(test_nid_less);
	test(test_nid_ostream);
	test(test_nid_fromstr);
	return test_fail_count();
}
