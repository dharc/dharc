/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_FABRIC_H_
#define FDSB_FABRIC_H_

#include <memory>
#include <forward_list>
#include <list>
#include <vector>
#include <chrono>
#include <utility>
#include <iterator>

#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

using std::forward_list;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::chrono::time_point;
using std::pair;
using std::list;
using std::size_t;

namespace fdsb {

struct TailHash {
	public:
	size_t operator()(const pair<Nid, Nid> &x) const {
		return x.first.i*3 + x.second.i;
	}
};

struct NidHash {
	public:
	size_t operator()(const Nid &x) const {
		return x.i;
	}
};

class Fabric {
	friend Harc;

	public:
	/**
	 * A list of Harc changes since this function was last called. The change
	 * log is reset when this function is called.
	 */
	unique_ptr<forward_list<const Harc*>> changes();

	/**
	 * Array of all Harcs that the given Nid is involved in.
	 * This array is intended to be sorted by significance, but since
	 * significance always changes there is no guarentee that it is up-to-date.
	 */
	const list<Harc*> &partners(const Nid &);

	/**
	 * Lookup a Harc using a pair of tail nodes.
	 */
	Harc &get(const Nid &a, const Nid &b) {
		return get((a < b) ? pair<Nid, Nid>(a, b) : pair<Nid, Nid>(b, a));
	}
	Harc &get(const pair<Nid, Nid> &key);

	/**
	 * Evaluate a normalised path through the fabric. If a dependant Harc is
	 * given then it will be added as a dependant to all visited Harcs in the
	 * path.
	 * @param p Normalised nested path through fabric.
	 * @param dep The Harc to add as dependant on this path.
	 * @return Result of following the normalised path p.
	 */
	Nid path(const vector<vector<Nid>> &p, const Harc *dep = nullptr);

	/**
	 * Evaluate several simple paths in parallel. If dep is given then it is
	 * added as a dependant on each of the paths.
	 * @param p Set of simple paths to evaluate separately.
	 * @param res Vector to put each result into.
	 * @param dep Harc to add as dependant on each path.
	 */
	void paths(const vector<vector<Nid>> &p,
			Nid *res,
			const Harc *dep = nullptr);

	/**
	 * Get the fabric singleton.
	 */
	static Fabric &singleton();

	/**
	 * Number of ticks since program start. Used to record when a relation
	 * was last accessed or changed.
	 */
	static unsigned long long counter() { return s_counter; }

	/**
	 * Number of milliseconds per tick.
	 */
	constexpr static unsigned long long counter_resolution() { return 100; }

	constexpr static int sig_prop_max() { return 20; }

	private:
	Fabric();
	~Fabric();
	Nid path_s(const vector<Nid> &, const Harc *dep = nullptr);
	static bool path_r(
			const vector<vector<Nid>> &p,
			Nid *res,
			int s, int e,
			const Harc *dep);
	void log_change(const Harc *h);
	static void counter_thread();

	unordered_map<pair<Nid, Nid>, Harc*, TailHash> m_harcs;
	unique_ptr<forward_list<const Harc*>> m_changes;
	unordered_map<Nid, list<Harc*>, NidHash> m_partners;

	static std::atomic<unsigned long long> s_counter;
};

extern Fabric &fabric;

inline auto begin(const Nid &n) {
	return fabric.partners(n).begin();
}

inline auto end(const Nid &n) {
	return fabric.partners(n).end();
}

};  // namespace fdsb

#endif /* FDSB_FABRIC_H_ */
