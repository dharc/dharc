#include "lest.hpp"
#include "dharc/patch.hpp"
#include <chrono>

#define BEGIN_PERF auto tstart = std::chrono::high_resolution_clock::now();
#define END_PERF(A, B) auto tend = std::chrono::high_resolution_clock::now(); \
	auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart); \
	std::cout << __func__ << ": " << ((A) / time_span.count()) << B << "\n";

const lest::test specification[] = {
CASE( "Process Performance" ) {
	auto *patch = new dharc::Patch<500,500,100,5,10,10>();

	BEGIN_PERF;	
	for (auto i = 0; i < 100; ++i) patch->process();
	END_PERF(100, "ps");
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}

