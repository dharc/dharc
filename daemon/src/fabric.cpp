#include "fdsb/fabric.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

Harc &Nid::operator[](const Nid &n)
{
	return fdsb::get(*this,n);
}

Harc &Harc::operator[](const Nid &n)
{
	return fdsb::get(query(),n);
}
