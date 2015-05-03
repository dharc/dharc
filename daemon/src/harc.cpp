#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using namespace fdsb;

Harc::Harc(const Nid &a, const Nid &b)
{
	m_tail[0] = a;
	m_tail[1] = b;
	m_head = null_nid;
	m_out_of_date = false;
}

const Nid &Harc::query()
{
	//Check if out_of_date.
	return m_head;
}

void Harc::add_dependant(Harc &h)
{
	m_dependants.push(&h);
}

void Harc::mark()
{
	m_out_of_date = true;
	//Mark all dependants.
}

void Harc::define(const Nid &n)
{
	m_head = n;
	m_out_of_date = false;
	//Mark all dependants.
}

Harc &Harc::operator=(const Nid &n)
{
	define(n);
	return *this;
}
