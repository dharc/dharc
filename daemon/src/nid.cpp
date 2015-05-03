#include <fdsb/nid.hpp>
#include <atomic>

std::atomic<unsigned long long> last_nid(0);

static Nid Nid::unique()
{
	return {Nid::Type::allocated,last_nid++};
}
