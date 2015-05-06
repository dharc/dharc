#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

/* Symetric Tail Test:
 * The order of tail nodes should be irrelevant, so lookup a Harc using both
 * orders and check the result is the same.
 */
void test_harc_symetric()
{
	get(10_n,11_n) = 55_n;
	CHECK(get(10_n,11_n) == 55_n);
	CHECK(get(11_n,10_n) == 55_n);
	get(11_n,10_n) = 66_n;
	CHECK(get(10_n,11_n) == 66_n);
	CHECK(get(11_n,10_n) == 66_n);
	
	// Check with different Nid types
	get('a'_n, 33_n).define('b'_n);
	CHECK(get('a'_n, 33_n) == 'b'_n);
	CHECK(get(33_n, 'a'_n) == 'b'_n);
	DONE;
}

/* Automatic new Harc creation upon get:
 * Request a Harc that does not exist. It should be created and returned.
 */
void test_harc_autocreate()
{
	get(15_n,16_n) = 67_n;
	CHECK(get(15_n,16_n) == 67_n);
	DONE;
}

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
	Harc *h1 = &Harc::get(123_n,'g'_n);
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
	Harc *h1 = &Harc::get(44_n,55_n);
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
	Harc *h1 = &Harc::get(33_n, 22_n);
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

/* Test that single non-nested paths evaluate correctly */
void test_harc_path()
{
	1_n[2_n] = Nid::unique();
	1_n[2_n][3_n] = Nid::unique();
	1_n[2_n][3_n][4_n] = 55_n;
	
	// Normal case
	CHECK(Harc::path({{1_n,2_n,3_n,4_n}}) == 55_n);
	// Degenerate case 1
	CHECK(Harc::path({{}}) == null_n);
	// Degenerate case 2
	CHECK(Harc::path({}) == null_n)
	// Base case
	CHECK(Harc::path({{5_n}}) == 5_n);
	DONE;
}

/* Check the parallel evaluation of more than one path */
void test_harc_paths()
{
	220_n[2_n] = Nid::unique();
	220_n[2_n][3_n] = Nid::unique();
	220_n[2_n][3_n][4_n] = 66_n;
	221_n[2_n] = Nid::unique();
	221_n[2_n][66_n] = 77_n;
	
	std::vector<Nid> res(10);
	
	// Normal case, 2 paths
	Harc::paths({
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	CHECK(res[0] == 77_n);
	CHECK(res[1] == 66_n);
	
	// Normal case, 3 paths
	Harc::paths({
		{221_n, 2_n, 66_n},
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	CHECK(res[0] == 77_n);
	CHECK(res[1] == 77_n);
	CHECK(res[2] == 66_n);
	
	// Degen case, 3 paths
	Harc::paths({
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
void test_harc_agregatepaths()
{
	10_n[2_n] = Nid::unique();
	10_n[2_n][3_n] = Nid::unique();
	10_n[2_n][3_n][4_n] = 66_n;
	11_n[2_n] = Nid::unique();
	11_n[2_n][66_n] = 77_n;
	
	CHECK(Harc::path({{11_n,2_n},{10_n,2_n,3_n,4_n}}) == 77_n);
	DONE;
}

void test_harc_concurrentdef() {
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

void test_harc_duplicateeval() {
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

int main(int argc, char *argv[]) {
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	test(test_harc_autocreate);
	test(test_harc_subscript);
	test(test_harc_symetric);
	test(test_harc_path);
	test(test_harc_paths);
	test(test_harc_agregatepaths);
	test(test_harc_definition);
	test(test_harc_dependency);
	test(test_harc_concurrentdef);
	test(test_harc_duplicateeval);
	return test_fail_count();
}
