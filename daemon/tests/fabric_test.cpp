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
	CHECK(get(1_nid,2_nid) == 47_nid);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_fabric_addget);
	return 0;
}
