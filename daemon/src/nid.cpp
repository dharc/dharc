#include "fdsb/nid.hpp"
#include <atomic>

using namespace fdsb;

std::atomic<unsigned long long> last_nid(0);

Nid Nid::unique()
{
	return {Nid::Type::allocated,last_nid++};
}
