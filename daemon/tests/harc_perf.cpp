#include "fdsb/harc.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/test.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono;
using namespace fdsb;

void read_performance() {
	// Set up harc.
	1_n[2_n] = 3_n;
	
	BEGIN_PERF
	
	for (int i = 0; i < 10000000; ++i) {
		Harc::get(1_n,2_n).query();
	}
	
	END_PERF(10,"Mps")
}

int main(int argc, char *argv[]) {
	read_performance();
	return 0;
}
