#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/query.hpp"

using namespace fdsb;

Harc::Harc(const Nid &a, const Nid &b)
{
	m_tail[0] = a;
	m_tail[1] = b;
	m_head = null_n;
}

void Harc::add_dependant(Harc &h)
{
	m_dependants.push_back(&h);
}

void Harc::update()
{
	m_head = path(m_def,this);
}

void Harc::dirty()
{
	mark_as_dirty(this);
	for (auto i : m_dependants)
	{
		i->dirty();
	}
	m_dependants.clear();
}

void Harc::define(const Nid &n)
{
	m_head = n;
	for (auto i : m_dependants)
	{
		i->dirty();
	}
	m_dependants.clear();
}
	
void Harc::define(const std::vector<std::vector<Nid>> &p)
{
	m_def = p;
	dirty();
}

Harc &Harc::operator=(const Nid &n)
{
	define(n);
	return *this;
}

bool Harc::operator==(const Nid &n)
{
	return query() == n;
}

Harc &Harc::get(const Nid &a, const Nid &b)
{
	Harc *h;
	auto range = s_fabric.equal_range(Nid::dual_hash(a,b));
	
	for (auto i = range.first; i != range.second; i++)
	{
		h = i->second;
		if (h->equal_tail(a,b))
		{
			return *h;
		}
	}
	
	h = new Harc(a,b);
	s_fabric.insert({{Nid::dual_hash(a,b), h}});
}

Nid Harc::path(const std::vector<Nid> &p, Harc *dep)
{
	if (p.size() > 1)
	{
		Nid temp = p[0];
		for (auto i = ++p.begin(); i != p.end(); ++i)
		{
			Harc &h = Harc::get(temp,*i);
			if (dep)
			{
				h.add_dependant(*dep);
			}
			temp = h.query();
		}
		return temp;
	}
	else if (p.size() == 1)
	{
		return p[0];
	}
	else
	{
		return null_n;
	}
}

Nid Harc::path(const std::vector<std::vector<Nid>> &p, Harc *dep)
{
	int ix = 0;
	std::vector<Nid> res(p.size());
	
	//These can all be done in different threads!
	for (auto i : p)
	{
		res[ix++] = Harc::path(i,dep);
	}
	//Final recombination
	return Harc::path(res,dep);
}
