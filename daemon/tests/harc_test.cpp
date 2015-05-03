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

int main(int argc, char *argv[])
{
	test(test_harc_defquery);
	return 0;
}
