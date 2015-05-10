/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_FABRIC_H_
#define FDSB_FABRIC_H_

#include <memory>
#include <forward_list>
#include <vector>
#include <chrono>
#include <utility>

#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using std::forward_list;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::chrono::time_point;
using std::pair;
using std::size_t;

namespace fdsb {

struct TailHash {
	public:
	size_t operator()(const pair<Nid, Nid> &x) const {
		return x.first.i*3 + x.second.i;
	}
};

class Fabric {
	friend Harc;

	public:
	/**
	 * A list of Harc changes since this function was last called. The change
	 * log is reset when this function is called.
	 */
	unique_ptr<forward_list<Harc*>> changes();

	/**
	 * Array of all Nids that the given Nid is paired with in Harc tails.
	 */
	const vector<Nid> &partners(const Nid &);

	/**
	 * Lookup a Harc using a pair of tail nodes.
	 */
	Harc &get(const Nid &, const Nid &);

	/**
	 * Evaluate a normalised path through the fabric.
	 */
	Nid path(const Path &, Harc *dep = nullptr);

	/**
	 * Evaluate several simple paths in parallel.
	 */
	void paths(const Path &, Nid *res, Harc *dep = nullptr);

	/**
	 * Get the fabric singleton.
	 */
	static Fabric &singleton();

	private:
	Fabric();
	~Fabric();
	Nid path_s(const vector<Nid> &, Harc *dep = nullptr);
	static bool path_r(const Path &p, Nid *res, int s, int e, Harc *dep);
	void log_change(Harc *h);

	unordered_map<pair<Nid, Nid>, Harc*, TailHash> m_harcs;
	unique_ptr<forward_list<Harc*>> m_changes;
};

extern Fabric &fabric;

};  // namespace fdsb

#endif /* FDSB_FABRIC_H_ */
