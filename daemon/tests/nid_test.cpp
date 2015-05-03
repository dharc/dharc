#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"

using namespace fdsb;

void test_nid_ilit()
{
	Nid n1 = 123_nid;
	CHECK(n1.t == Nid::Type::integer);
	CHECK(n1.i == 123);
	DONE;
}

int main(int argc, char *argv[])
{
	fdsb_test(test_nid_ilit);
	return 0;
}
