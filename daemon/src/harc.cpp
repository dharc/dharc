#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

Harc::Harc(const Nid &a, const Nid &b)
{
	tail[0] = a;
	tail[1] = b;
	head = null_nid;
	out_of_date = false;
}

const Nid &Harc::query()
{
	//Check if out_of_date.
	return head;
}

void Harc::add_dependant(Harc &h)
{
	dependants.push(&h);
}

void Harc::mark()
{
	out_of_date = true;
	//Mark all dependants.
}

void Harc::define(const Nid &n)
{
	head = n;
	out_of_date = false;
	//Mark all dependants.
}

Harc &Harc::operator=(const Nid &n)
{
	define(n);
	return *this;
}
