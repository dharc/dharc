#include "fdsb/query.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

Nid fdsb::path(const std::vector<Nid> &p, Harc *dep)
{
	if (p.size() > 1)
	{
		Nid temp = p[0];
		for (auto i = ++p.begin(); i != p.end(); ++i)
		{
			Harc &h = get(temp,*i);
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

Nid fdsb::path(const std::vector<std::vector<Nid>> &p, Harc *dep)
{
	int ix = 0;
	std::vector<Nid> res(p.size());
	
	//These can all be done in different threads!
	for (auto i : p)
	{
		res[ix++] = path(i,dep);
	}
	//Final recombination
	return path(res,dep);
}
