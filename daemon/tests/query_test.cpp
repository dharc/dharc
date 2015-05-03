#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"
#include "fdsb/query.hpp"

using namespace fdsb;

void test_query_path()
{
	1_nid[2_nid] = Nid::unique();
	1_nid[2_nid][3_nid] = 55_nid;
	
	CHECK(path({1_nid,2_nid,3_nid}) == 55_nid);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_query_path);
	return 0;
}
