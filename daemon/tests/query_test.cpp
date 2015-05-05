#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"
#include "fdsb/query.hpp"

using namespace fdsb;

void test_query_path()
{
	1_n[2_n] = Nid::unique();
	1_n[2_n][3_n] = Nid::unique();
	1_n[2_n][3_n][4_n] = 55_n;
	
	CHECK(Harc::path({1_n,2_n,3_n,4_n}) == 55_n);
	DONE;
}

void test_query_paths()
{
	10_n[2_n] = Nid::unique();
	10_n[2_n][3_n] = Nid::unique();
	10_n[2_n][3_n][4_n] = 66_n;
	11_n[2_n] = Nid::unique();
	11_n[2_n][66_n] = 77_n;
	
	CHECK(Harc::path({{11_n,2_n},{10_n,2_n,3_n,4_n}}) == 77_n);
	DONE;
}

int main(int argc, char *argv[])
{
	test(test_query_path);
	test(test_query_paths);
	return test_fail_count();
}
