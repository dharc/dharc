#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

/* ==== MOCKS =============================================================== */

Harc::Harc(const pair<Nid, Nid> &t) :
	m_tail(t),
	m_head(null_n),
	m_flags(Flag::none),
	m_lastquery(Fabric::counter()),
	m_strength(0.0) {}

void Harc::add_dependant(Harc &h) {
	m_dependants.push_back(&h);
}

const Nid &Harc::query() {
	return m_head;
}

void Harc::define(const Nid &n) {
	if (check_flag(Flag::log)) fabric.log_change(this);

	m_head = n;
}

Harc &Harc::operator=(const Nid &n) {
	define(n);
	return *this;
}

bool Harc::operator==(const Nid &n) {
	return query() == n;
}

/* ==== END MOCKS =========================================================== */


/* Symetric Tail Test:
 * The order of tail nodes should be irrelevant, so lookup a Harc using both
 * orders and check the result is the same.
 */
void test_fabric_symetric()
{
	fabric.get(10_n,11_n) = 55_n;
	CHECK(fabric.get(10_n,11_n) == 55_n);
	CHECK(fabric.get(11_n,10_n) == 55_n);
	fabric.get(11_n,10_n) = 66_n;
	CHECK(fabric.get(10_n,11_n) == 66_n);
	CHECK(fabric.get(11_n,10_n) == 66_n);
	
	// Check with different Nid types
	fabric.get('a'_n, 33_n).define('b'_n);
	CHECK(fabric.get('a'_n, 33_n) == 'b'_n);
	CHECK(fabric.get(33_n, 'a'_n) == 'b'_n);
	DONE;
}

/* Automatic new Harc creation upon get:
 * Request a Harc that does not exist. It should be created and returned.
 */
void test_fabric_autocreate()
{
	fabric.get(15_n,16_n) = 67_n;
	CHECK(fabric.get(15_n,16_n) == 67_n);
	DONE;
}

/* Test that single non-nested paths evaluate correctly */
void test_fabric_path()
{
	fabric.get(1_n, 2_n).define(1000_n);
	fabric.get(1000_n, 3_n).define(1001_n);
	fabric.get(1001_n, 4_n).define(55_n);
	
	// Normal case
	CHECK(fabric.path({{1_n,2_n,3_n,4_n}}) == 55_n);
	// Degenerate case 1
	CHECK(fabric.path({{}}) == null_n);
	// Degenerate case 2
	CHECK(fabric.path({}) == null_n)
	// Base case
	CHECK(fabric.path({{5_n}}) == 5_n);
	DONE;
}

/* Check the parallel evaluation of more than one path */
void test_fabric_paths()
{
	fabric.get(220_n, 2_n).define(2000_n);
	fabric.get(2000_n, 3_n).define(2001_n);
	fabric.get(2001_n, 4_n).define(66_n);
	fabric.get(221_n, 2_n).define(2002_n);
	fabric.get(2002_n, 66_n).define(77_n);
	
	std::vector<Nid> res(10);
	
	// Normal case, 2 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	CHECK(res[0] == 77_n);
	CHECK(res[1] == 66_n);
	
	// Normal case, 3 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	CHECK(res[0] == 77_n);
	CHECK(res[1] == 77_n);
	CHECK(res[2] == 66_n);
	
	// Degen case, 3 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	CHECK(res[0] == 77_n);
	CHECK(res[1] == null_n);
	CHECK(res[2] == 66_n);
	DONE;
}

/* Check that nested (but normalised) paths evaluate correctly */
void test_fabric_agregatepaths()
{
	fabric.get(10_n, 2_n).define(3000_n);
	fabric.get(3000_n, 3_n).define(3001_n);
	fabric.get(3001_n, 4_n).define(66_n);
	fabric.get(11_n, 2_n).define(3002_n);
	fabric.get(3002_n, 66_n).define(77_n);
	
	CHECK(fabric.path({{11_n,2_n},{10_n,2_n,3_n,4_n}}) == 77_n);
	DONE;
}

void test_fabric_duplicateeval() {
	//Main chain
	fabric.get(2000_n, 2_n).define(5000_n);
	fabric.get(5000_n, 3_n).define(5001_n);
	fabric.get(5001_n, 4_n).define(5002_n);
	fabric.get(5002_n, 5_n).define(5003_n);
	fabric.get(5003_n, 6_n).define(5004_n);
	fabric.get(5004_n, 7_n).define(5005_n);
	fabric.get(5005_n, 8_n).define(10_n);
	
	//Result chain
	fabric.get(10_n, 10_n).define(6000_n);
	fabric.get(6000_n, 10_n).define(6001_n);
	fabric.get(6001_n, 10_n).define(6002_n);
	fabric.get(6002_n, 10_n).define(99_n);
	
	CHECK(fabric.path({
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n}
	}) == 99_n);
	DONE;
}

void test_fabric_changes() {
	auto chg = fabric.changes();
	fabric.get(40_n,41_n).set_flag(Harc::Flag::log);
	fabric.get(40_n,42_n).set_flag(Harc::Flag::log);
	fabric.get(40_n,41_n).define(23_n);
	fabric.get(40_n,42_n).define(24_n);
	chg = fabric.changes();
	
	CHECK(chg.get() != nullptr);
	if (chg.get()) {
		CHECK(!chg->empty());
		if (!chg->empty()) {
			CHECK(chg->front()->query() == 24_n);
			chg->pop_front();
		}
		CHECK(!chg->empty());
		if (!chg->empty()) {
			CHECK(chg->front()->query() == 23_n);
		}
	}
	
	chg = fabric.changes();
	CHECK(chg->empty());
	
	DONE;
}

void test_fabric_partnersexist() {
	Nid n1 = 333_n;

	fabric.get(n1, 34_n).define(78_n);
	fabric.get(n1, 35_n).define(79_n);
	fabric.get(n1, 36_n).define(80_n);

	auto partners = fabric.partners(n1);
	CHECK(partners.size() == 3);
	
	for (auto i : partners) {
		CHECK(i->tail_contains(34_n) || i->tail_contains(35_n) || i->tail_contains(36_n));
	}
	
	DONE;
}

int main(int argc, char *argv[]) {
	test(test_fabric_symetric);
	test(test_fabric_path);
	test(test_fabric_paths);
	test(test_fabric_agregatepaths);
	test(test_fabric_duplicateeval);
	test(test_fabric_changes);
	test(test_fabric_partnersexist);
	return test_fail_count();
}

