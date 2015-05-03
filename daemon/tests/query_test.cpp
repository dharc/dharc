#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"
#include "fdsb/query.hpp"

using namespace fdsb;

void test_query_path()
{
	1_nid[2_nid] = Nid::unique();
	1_nid[2_nid][3_nid] = Nid::unique();
	1_nid[2_nid][3_nid][4_nid] = 55_nid;
	
	CHECK(path({1_nid,2_nid,3_nid,4_nid}) == 55_nid);
	DONE;
}

void test_query_paths()
{
	10_nid[2_nid] = Nid::unique();
	10_nid[2_nid][3_nid] = Nid::unique();
	10_nid[2_nid][3_nid][4_nid] = 66_nid;
	11_nid[2_nid] = Nid::unique();
	11_nid[2_nid][66_nid] = 77_nid;
	
	CHECK(path({{11_nid,2_nid},{10_nid,2_nid,3_nid,4_nid}}) == 77_nid);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_query_path);
	test(test_query_paths);
	return 0;
}
