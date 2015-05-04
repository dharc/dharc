#ifndef _FDSB_QUERY_H_
#define _FDSB_QUERY_H_

#include "fdsb/nid.hpp"
#include <vector>

namespace fdsb
{
	
class Harc;
	
//std::set<Nid> related(Nid &, int count);

/**
 * Evaluate a list of nodes as a simple path through the fabric.
 */
Nid path(const std::vector<Nid> &, Harc *dep=nullptr);

/**
 * Evaluate a list of paths and then combine results as a path.
 * This function evaluates each sub-path in parallel before recombining.
 */
Nid path(const std::vector<std::vector<Nid>> &, Harc *dep=nullptr);

};

#endif /* _FDSB_QUERY_H_ */
