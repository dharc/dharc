#ifndef _FDSB_QUERY_H_
#define _FDSB_QUERY_H_

#include "fdsb/nid.hpp"
#include <vector>

namespace fdsb
{
	
//std::set<Harc *> related(Harc &h, int count);
Nid path(const std::vector<Nid> &);
Nid path(const std::vector<std::vector<Nid>> &);

};

#endif /* _FDSB_QUERY_H_ */
