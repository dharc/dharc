#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
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

Nid dummy_result;

Nid Fabric::path(const Path &p, Harc *dep) {
	return dummy_result;
}

Harc &Fabric::get(const Nid &a, const Nid &b) {
	Harc *h;
	auto range = m_harcs.equal_range(Nid::dual_hash(a, b));

	// Find the exact Harc in the bucket.
	for (auto i = range.first; i != range.second; ++i) {
		h = i->second;
		if (h->equal_tail(a, b)) {
			return *h;
		}
	}

	// Does not exist, so make it.
	h = new Harc(a, b);
	m_harcs.insert({{Nid::dual_hash(a, b), h}});
	return *h;
}

/* ==== END MOCKS ==== */

void test_harc_eqtail() {
	Harc &h1 = fabric.get(125_n, 'j'_n);
	CHECK(h1.equal_tail('j'_n, 125_n));
	CHECK(h1.equal_tail(125_n, 'j'_n));
	CHECK(!(h1.equal_tail(126_n, 'j'_n)));
	DONE;
}

/* Subscript operator overload test:
 * Does the subscript operator for Nid and Harc work as expected.
 */
void test_harc_subscript()
{
	Harc &h1 = 19_n[20_n];
	Harc &h2 = 20_n[19_n];
	
	CHECK(h1.equal_tail(h2.tail<0>(),h2.tail<1>()));
	DONE;
}

/* Basic Harc creation, definition and query:
 * Make sure Harcs are created correctly and the a constant define and
 * associated query match.
 */
void test_harc_defquery()
{
	Harc &h1 = fabric.get(123_n,'g'_n);
	h1.define(55_n);
	CHECK(h1.is_out_of_date() == false);
	CHECK(h1.query() == 55_n);
	h1.define(77_n);
	CHECK(h1.is_out_of_date() == false);
	CHECK(h1.query() == 77_n);
	DONE;
}

/* Check Nid assignment operator:
 * Does assigning a Nid to a Harc perform a constant define.
 */
void test_harc_assign()
{
	Harc &h1 = fabric.get(44_n,55_n);
	h1 = 66_n;
	CHECK(h1.query() == 66_n);
	h1 = 88_n;
	CHECK(h1.query() == 88_n);
	DONE;
}

/* Nid Harc equality operator:
 * If a Harc is compared with a Nid for equality, it should compare the head
 * with the Nid.
 */
void test_harc_eqnid()
{
	Harc h1 = fabric.get(33_n, 22_n);
	h1 = 78_n;
	CHECK(h1 == 78_n);
	CHECK(!(h1 == 'a'_n));
	DONE;
}

/* Check a basic definition
 */
void test_harc_definition()
{
	Harc &h1 = 102_n[103_n];
	
	h1.define({{100_n,101_n}});
	CHECK(h1.is_out_of_date());
	dummy_result = 49_n;
	CHECK(h1.query() == 49_n);
	CHECK(102_n[103_n].is_out_of_date() == false);
	
	dummy_result = 50_n;
	CHECK(h1.query() == 49_n);
	
	h1.dirty();
	CHECK(h1.is_out_of_date());
	CHECK(h1.query() == 50_n);
	DONE;
}

void test_harc_dependency()
{
	Harc &h1 = 100_n[101_n];
	Harc &h2 = 102_n[103_n];
	
	dummy_result = 43_n;
	h1.define({{43_n}});
	CHECK(h1.query() == 43_n);
	CHECK(h1.is_out_of_date() == false);
	
	h2.add_dependant(h1);
	h2.define(65_n);
	CHECK(h1.is_out_of_date());
	DONE;
}

int main(int argc, char *argv[]) {
	test(test_harc_eqtail);
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	test(test_harc_subscript);
	test(test_harc_definition);
	test(test_harc_dependency);
	return test_fail_count();
}
