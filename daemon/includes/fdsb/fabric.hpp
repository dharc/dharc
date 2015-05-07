/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_FABRIC_H_
#define FDSB_FABRIC_H_

#include <memory>
#include <list>
#include <vector>

#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using std::shared_ptr;
using std::list;
using std::vector;
using std::unordered_multimap;

namespace fdsb {

class Fabric {
	public:
	shared_ptr<list<Harc*>*> changes();
	const list<Harc*> &node_harcs();
	Harc &get(const Nid &, const Nid &);
	Nid path(const Path &, Harc *dep = nullptr);
	void paths(const Path &, Nid *res, Harc *dep = nullptr);

	static Fabric &singleton();

	private:
	Fabric();
	~Fabric();
	Nid path_s(const vector<Nid> &, Harc *dep = nullptr);
	static bool path_r(const Path &p, Nid *res, int s, int e, Harc *dep);

	unordered_multimap<unsigned long long, Harc*> m_harcs;
};

extern Fabric &fabric;

};  // namespace fdsb

#endif /* FDSB_FABRIC_H_ */
