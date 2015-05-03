#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

void test_fabric_addget()
{
	Harc h1(1_nid,2_nid);
	add(h1);
	get(1_nid,2_nid) = 47_nid;
	add(3_nid,4_nid);
	get(3_nid,4_nid) = 56_nid;
	CHECK(get(1_nid,2_nid) == 47_nid);
	CHECK(get(3_nid,4_nid) == 56_nid);
	DONE;
}

void test_fabric_symetric()
{
	add(10_nid,11_nid);
	get(10_nid,11_nid) = 55_nid;
	CHECK(get(10_nid,11_nid) == 55_nid);
	CHECK(get(11_nid,10_nid) == 55_nid);
	get(11_nid,10_nid) = 66_nid;
	CHECK(get(10_nid,11_nid) == 66_nid);
	CHECK(get(11_nid,10_nid) == 66_nid);
	DONE;
}

void test_fabric_autocreate()
{
	get(15_nid,16_nid) = 67_nid;
	CHECK(get(15_nid,16_nid) == 67_nid);
	DONE;
}

void test_fabric_subscript()
{
	19_nid[20_nid] = 21_nid;
	CHECK(19_nid[20_nid] == 21_nid);
	CHECK(20_nid[19_nid] == 21_nid);
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
