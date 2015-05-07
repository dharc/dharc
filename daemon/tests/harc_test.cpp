#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;


/* Subscript operator overload test:
 * Does the subscript operator for Nid and Harc work as expected.
 */
void test_harc_subscript()
{
	19_n[20_n] = 21_n;
	CHECK(19_n[20_n] == 21_n);
	CHECK(20_n[19_n] == 21_n);
	DONE;
}

/* Basic Harc creation, definition and query:
 * Make sure Harcs are created correctly and the a constant define and
 * associated query match.
 */
void test_harc_defquery()
{
	Harc *h1 = &fabric.get(123_n,'g'_n);
	h1->define(55_n);
	CHECK(h1->equal_tail('g'_n,123_n));
	CHECK(h1->query() == 55_n);
	DONE;
}

/* Check Nid assignment operator:
 * Does assigning a Nid to a Harc perform a constant define.
 */
void test_harc_assign()
{
	Harc *h1 = &fabric.get(44_n,55_n);
	*h1 = 66_n;
	CHECK(h1->query() == 66_n);
	DONE;
}

/* Nid Harc equality operator:
 * If a Harc is compared with a Nid for equality, it should compare the head
 * with the Nid.
 */
void test_harc_eqnid()
{
	Harc *h1 = &fabric.get(33_n, 22_n);
	*h1 = 78_n;
	CHECK(*h1 == 78_n);
	CHECK(!(*h1 == 'a'_n));
	DONE;
}

/* Check a basic definition
 */
void test_harc_definition()
{
	100_n[101_n] = 49_n;
	102_n[103_n].define({{100_n,101_n}});
	CHECK(102_n[103_n].is_out_of_date());
	CHECK(102_n[103_n].query() == 49_n);
	CHECK(102_n[103_n].is_out_of_date() == false);
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

int main(int argc, char *argv[]) {
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	test(test_harc_subscript);
	test(test_harc_definition);
	test(test_harc_dependency);
	return test_fail_count();
}
