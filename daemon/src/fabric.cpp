#include "fdsb/fabric.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

#include <unordered_map>

using namespace fdsb;

std::unordered_multimap<unsigned long long,Harc*> fabric;

Harc &fdsb::get(const Nid &a, const Nid &b)
{
	auto range = fabric.equal_range(Nid::dual_hash(a,b));
	for (auto i = range.first; i != range.second; i++)
	{
		if (((*i)->tail(0) == a && (*i)->tail(1) == b) || ((*i)->tail(0) == b && (*i)->tail(1) == a))
		{
			return *(*i);
		}
	}
	
	Harc *nh = new Harc(a,b);
	add(*nh);
	return *nh;
}

void fdsb::add(Harc &h)
{
	fabric.insert({{Nid::dual_hash(h.tail(0),h.tail(1)), &h}});
}

void fdsb::add(const Nid &n1, const Nid &n2)
{
	fdsb::add(*(new Harc(n1,n2)));
}

Harc &Nid::operator[](const Nid &n)
{
	return fdsb::get(*this,n);
}

Harc &Harc::operator[](const Nid &n)
{
	return fdsb::get(query(),n);
}
