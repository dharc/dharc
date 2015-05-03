#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

void test_harc_defquery()
{
	Harc h1(123_nid,999_nid);
	h1.define(55_nid);
	CHECK(h1.tail(0) == 123_nid);
	CHECK(h1.tail(1) == 999_nid);
	CHECK(h1.query() == 55_nid);
	DONE;
}

void test_harc_assign()
{
	Harc h1(44_nid,55_nid);
	h1 = 66_nid;
	CHECK(h1.query() == 66_nid);
	DONE;
}

void test_harc_eqnid()
{
	Harc h1(33_nid, 22_nid);
	h1 = 78_nid;
	CHECK(h1 == 78_nid);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	return 0;
}
