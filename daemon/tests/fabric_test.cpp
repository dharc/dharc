#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

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
	1_n[2_n] = Nid::unique();
	1_n[2_n][3_n] = Nid::unique();
	1_n[2_n][3_n][4_n] = 55_n;
	
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
	220_n[2_n] = Nid::unique();
	220_n[2_n][3_n] = Nid::unique();
	220_n[2_n][3_n][4_n] = 66_n;
	221_n[2_n] = Nid::unique();
	221_n[2_n][66_n] = 77_n;
	
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
	10_n[2_n] = Nid::unique();
	10_n[2_n][3_n] = Nid::unique();
	10_n[2_n][3_n][4_n] = 66_n;
	11_n[2_n] = Nid::unique();
	11_n[2_n][66_n] = 77_n;
	
	CHECK(fabric.path({{11_n,2_n},{10_n,2_n,3_n,4_n}}) == 77_n);
	DONE;
}

void test_fabric_concurrentdef() {
	// First chain
	1000_n[2_n] = Nid::unique();
	1000_n[2_n][3_n] = Nid::unique();
	1000_n[2_n][3_n][4_n] = 600_n;
	
	// Second chain
	1001_n[2_n] = Nid::unique();
	1001_n[2_n][3_n] = Nid::unique();
	1001_n[2_n][3_n][4_n] = 500_n;
	
	// Third chain
	1002_n[2_n] = Nid::unique();
	1002_n[2_n][3_n] = Nid::unique();
	1002_n[2_n][3_n][4_n] = 400_n;
	
	// Forth chain
	1003_n[2_n] = Nid::unique();
	1003_n[2_n][3_n] = Nid::unique();
	1003_n[2_n][3_n][4_n] = 300_n;
	
	// Fith chain
	1004_n[2_n] = Nid::unique();
	1004_n[2_n][3_n] = Nid::unique();
	1004_n[2_n][3_n][4_n] = 200_n;
	
	// First dependency chain
	1005_n[2_n] = Nid::unique();
	1005_n[2_n][3_n] = Nid::unique();
	1005_n[2_n][3_n][4_n].define({{1004_n, 2_n, 3_n, 4_n}});
	
	// Final chain
	600_n[500_n] = Nid::unique();
	600_n[500_n][400_n] = Nid::unique();
	600_n[500_n][400_n][300_n] = Nid::unique();
	600_n[500_n][400_n][300_n][200_n] = 555_n;
	
	// Test definition
	500_n[1_n].define({
		{1000_n, 2_n, 3_n, 4_n},
		{1001_n, 2_n, 3_n, 4_n},
		{1002_n, 2_n, 3_n, 4_n},
		{1003_n, 2_n, 3_n, 4_n},
		{1005_n, 2_n, 3_n, 4_n}
	});
	
	CHECK(500_n[1_n] == 555_n);
	DONE;
}

void test_fabric_duplicateeval() {
	//Main chain
	2000_n[2_n] = Nid::unique();
	2000_n[2_n][3_n] = Nid::unique();
	2000_n[2_n][3_n][4_n] = Nid::unique();
	2000_n[2_n][3_n][4_n][5_n] = Nid::unique();
	2000_n[2_n][3_n][4_n][5_n][6_n] = Nid::unique();
	2000_n[2_n][3_n][4_n][5_n][6_n][7_n] = Nid::unique();
	2000_n[2_n][3_n][4_n][5_n][6_n][7_n][8_n] = 10_n;
	
	//Result chain
	10_n[10_n] = Nid::unique();
	10_n[10_n][10_n] = Nid::unique();
	10_n[10_n][10_n][10_n] = Nid::unique();
	10_n[10_n][10_n][10_n][10_n] = 99_n;
	
	// Test definition
	2001_n[1_n].define({
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n}
	});
	
	CHECK(2001_n[1_n] == 99_n);
	DONE;
}

void test_fabric_changes() {
	auto chg = fabric.changes();
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

int main(int argc, char *argv[]) {
	test(test_fabric_symetric);
	test(test_fabric_path);
	test(test_fabric_paths);
	test(test_fabric_agregatepaths);
	test(test_fabric_concurrentdef);
	test(test_fabric_duplicateeval);
	test(test_fabric_changes);
	return test_fail_count();
}

