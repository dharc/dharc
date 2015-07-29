#include "dharc/element.hpp"
#include "lest.hpp"

#include <limits>
#include <iostream>
#include <chrono>

#define BEGIN_PERF auto tstart = std::chrono::high_resolution_clock::now();
#define END_PERF(A, B) auto tend = std::chrono::high_resolution_clock::now(); \
	auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart); \
	std::cout << __func__ << ": " << ((A) / time_span.count()) << B << "\n";

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) < std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

const lest::test specification[] = {
CASE( "Search for dead links" ) {
	dharc::Element<10> ele;
	vector<dharc::Link*> links = ele.deadLinks(5);
	EXPECT(links.size() == 5U);
	EXPECT(links[0] != nullptr);
	EXPECT(links[0]->strength == 0.0f);
},

CASE( "Element integration" ) {
	dharc::Element<10> ele;
	vector<dharc::Link*> links = ele.deadLinks(10);
	EXPECT( links.size() == 10U );

	for (auto l : links) {
		l->strength = 0.5;
		l->energy = 0.2f;
	}

	EXPECT( almost_equal(ele.integrate(), 1.0f, 4) );
	links[0]->energy = 1.0f;
	EXPECT( almost_equal(ele.integrate(), 0.6f, 4) );
},

CASE( "Positive Learning" ) {
	dharc::Element<10> ele;
	vector<dharc::Link*> links = ele.deadLinks(10);
	EXPECT( links.size() == 10U );

	float e = 0.1f;
	for (auto l : links) {
		l->strength = 0.5;
		l->energy = e;
		e -= 0.001f;
	}

	float expected = 0.5 + 0.1 * dharc::ElementCore::kLearnRate;
	ele.setFrequency(255);
	dharc::Element<10>::learn(static_cast<dharc::ElementCore*>(&ele));

	EXPECT( almost_equal(links[0]->strength, expected, 4) );
	EXPECT( links[9]->strength < 0.5f );
},

CASE( "Integration Performance" ) {
	dharc::Element<10000> ele;
	volatile float total = 0.0f;
	BEGIN_PERF;
	for (auto i = 0U; i < 10000; ++i) total += ele.integrate();
	END_PERF(100, "Mps");
},

CASE( "Learning Performance" ) {
	dharc::Element<10000> ele;
	BEGIN_PERF;
	for (auto i = 0U; i < 10000; ++i) {
		ele.reset();
		ele.setFrequency(100);
		dharc::Element<10000>::learn(&ele);
	}
	END_PERF(100, "Mps");
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}

