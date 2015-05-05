#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

void test_harc_defquery()
{
	Harc *h1 = &Harc::get(123_n,999_n);
	h1->define(55_n);
	CHECK(h1->equal_tail(123_n,999_n));
	CHECK(h1->query() == 55_n);
	DONE;
}

void test_harc_assign()
{
	Harc *h1 = &Harc::get(44_n,55_n);
	*h1 = 66_n;
	CHECK(h1->query() == 66_n);
	DONE;
}

void test_harc_eqnid()
{
	Harc *h1 = &Harc::get(33_n, 22_n);
	*h1 = 78_n;
	CHECK(*h1 == 78_n);
	DONE;
}

/*void test_harc_dependants()
{
	Harc h1 = Harc::get(22_n,23_n);
	Harc h2 = Harc::get(24_n,25_n);
	Harc h3 = Harc::get(26_n,27_n);
	h2.add_dependant(h1);
	h3.add_dependant(h2);
	
	CHECK(h1.is_out_of_date() == false);
	CHECK(h2.is_out_of_date() == false);
	CHECK(h3.is_out_of_date() == false);
	h3 = 44_n;
	CHECK(h3.is_out_of_date() == false);
	CHECK(h2.is_out_of_date() == true);
	CHECK(h1.is_out_of_date() == true);
	DONE;
}*/

void test_harc_definition()
{
	100_n[101_n] = 49_n;
	102_n[103_n].define({{100_n,101_n}});
	CHECK(102_n[103_n].query() == 49_n);
	DONE;
}

void test_harc_dependency()
{
	100_n[101_n] = 49_n;
	102_n[103_n].define({{100_n,101_n}});
	CHECK(102_n[103_n].query() == 49_n);
	100_n[101_n] = 56_n;
	CHECK(102_n[103_n].query() == 56_n);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	//test(test_harc_dependants);
	test(test_harc_definition);
	test(test_harc_dependency);
	return test_fail_count();
}
