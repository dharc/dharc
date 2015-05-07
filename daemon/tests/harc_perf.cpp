#include "fdsb/harc.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/test.hpp"
#include "fdsb/fabric.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono;
using namespace fdsb;

void read_performance() {
	// Set up harc.
	1_n[2_n] = 3_n;
	
	BEGIN_PERF
	
	for (int i = 0; i < 10000000; ++i) {
		fabric.get(1_n,2_n).query();
	}
	
	END_PERF(10,"Mps")
}

void parallel_paths() {
	// Set up base chain
	1_n[1_n] = 1_n;
	
	// Dummy result chain
	5_n[5_n] = 5_n;
	
	std::vector<std::vector<Nid>> def;
	for (int i = 0; i < 10000; ++i) {
		def.push_back({1_n, 1_n, 1_n, 1_n, 1_n, 1_n, 1_n});
	}
	
	BEGIN_PERF
	fabric.path(def);
	END_PERF(10000,"ps")
}

int main(int argc, char *argv[]) {
	read_performance();
	parallel_paths();
	return 0;
}
