#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"

using namespace fdsb;

void test_nid_ilit()
{
	Nid n1 = 123_nid;
	CHECK(n1.t == Nid::Type::integer);
	CHECK(n1.i == 123);
	DONE;
}

void test_nid_dlit()
{
	Nid n1 = 23.67_nid;
	CHECK(n1.t == Nid::Type::real);
	CHECK(n1.d > 23.66 && n1.d < 23.68);
	DONE;
}

void test_nid_clit()
{
	Nid n1 = 'f'_nid;
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
	Nid n1 = 1234_nid;
	Nid n2 = 99_nid;
	CHECK(!(n1 == n2));
	CHECK(n1 != n2);
	n2 = 1234_nid;
	CHECK(n1 == n2);
	CHECK(!(n1 != n2));
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_nid_ilit);
	test(test_nid_dlit);
	test(test_nid_clit);
	test(test_nid_unique);
	test(test_nid_eqne);
	return 0;
}
