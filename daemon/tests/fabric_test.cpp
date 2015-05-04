#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

void test_fabric_addget()
{
	Harc h1(1_n,2_n);
	add(h1);
	get(1_n,2_n) = 47_n;
	add(3_n,4_n);
	get(3_n,4_n) = 56_n;
	CHECK(get(1_n,2_n) == 47_n);
	CHECK(get(3_n,4_n) == 56_n);
	DONE;
}

void test_fabric_symetric()
{
	add(10_n,11_n);
	get(10_n,11_n) = 55_n;
	CHECK(get(10_n,11_n) == 55_n);
	CHECK(get(11_n,10_n) == 55_n);
	get(11_n,10_n) = 66_n;
	CHECK(get(10_n,11_n) == 66_n);
	CHECK(get(11_n,10_n) == 66_n);
	DONE;
}

void test_fabric_autocreate()
{
	get(15_n,16_n) = 67_n;
	CHECK(get(15_n,16_n) == 67_n);
	DONE;
}

void test_fabric_subscript()
{
	19_n[20_n] = 21_n;
	CHECK(19_n[20_n] == 21_n);
	CHECK(20_n[19_n] == 21_n);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_fabric_addget);
	test(test_fabric_symetric);
	test(test_fabric_autocreate);
	test(test_fabric_subscript);
	return 0;
}
