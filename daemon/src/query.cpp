#include "fdsb/query.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/fabric.hpp"

using namespace fdsb;

Nid fdsb::path(const std::vector<Nid> &p)
{
	if (p.size() > 1)
	{
		Nid temp = p[0];
		for (auto i = ++p.begin(); i != p.end(); ++i)
		{
			temp = get(temp,*i).query();
		}
		return temp;
	}
	else if (p.size() == 1)
	{
		return p[0];
	}
	else
	{
		return null_nid;
	}
}

Nid fdsb::path(const std::vector<std::vector<Nid>> &p)
{
	int ix = 0;
	std::vector<Nid> res(p.size());
	
	//These can all be done in different threads!
	for (auto i : p)
	{
		res[ix++] = path(i);
	}
	//Final recombination
	return path(res);
}
