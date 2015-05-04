#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

void test_harc_defquery()
{
	Harc h1(123_nid,999_nid);
	h1.define(55_nid);
	CHECK(h1.equal_tail(123_nid,999_nid));
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

void test_harc_dependants()
{
	Harc h1(22_nid,23_nid);
	Harc h2(24_nid,25_nid);
	Harc h3(26_nid,27_nid);
	h2.add_dependant(h1);
	h3.add_dependant(h2);
	
	CHECK(h1.is_out_of_date() == false);
	CHECK(h2.is_out_of_date() == false);
	CHECK(h3.is_out_of_date() == false);
	h3 = 44_nid;
	CHECK(h3.is_out_of_date() == false);
	CHECK(h2.is_out_of_date() == true);
	CHECK(h1.is_out_of_date() == true);
	DONE;
}

void test_harc_definition()
{
	100_nid[101_nid] = 49_nid;
	102_nid[103_nid].define({{100_nid,101_nid}});
	CHECK(102_nid[103_nid].query() == 49_nid);
	DONE;
}

void test_harc_dependency()
{
	100_nid[101_nid] = 49_nid;
	102_nid[103_nid].define({{100_nid,101_nid}});
	CHECK(102_nid[103_nid].query() == 49_nid);
	100_nid[101_nid] = 56_nid;
	CHECK(102_nid[103_nid].query() == 56_nid);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	test(test_harc_dependants);
	test(test_harc_definition);
	return 0;
}
