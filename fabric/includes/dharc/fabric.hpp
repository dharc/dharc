/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_FABRIC_HPP_
#define DHARC_FABRIC_HPP_

#include <memory>
#include <forward_list>
#include <list>
#include <vector>
#include <chrono>
#include <utility>
#include <iterator>
#include <atomic>
#include <unordered_map>

#include "dharc/node.hpp"

using std::forward_list;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::chrono::time_point;
using std::pair;
using std::list;
using std::size_t;

namespace dharc {

class Harc;

struct TailHash {
	public:
	size_t operator()(const pair<Node, Node> &x) const {
		return x.first.i*3 + x.second.i;
	}
};

struct NidHash {
	public:
	size_t operator()(const Node &x) const {
		return x.i;
	}
};

class Fabric {
	friend Harc;

	public:
	Fabric();
	~Fabric();
	/**
	 * A list of Harc changes since this function was last called. The change
	 * log is reset when this function is called.
	 */
	unique_ptr<forward_list<const Harc*>> changes();

	/**
	 * Array of all Harcs that the given Node is involved in.
	 * This array is intended to be sorted by significance, but since
	 * significance always changes there is no guarentee that it is up-to-date.
	 */
	const list<Harc*> &partners(const Node &);

	/**
	 * Lookup a Harc using a pair of tail nodes.
	 */
	Harc &get(const Node &a, const Node &b) {
		return get((a < b) ? pair<Node, Node>(a, b) : pair<Node, Node>(b, a));
	}

	Harc &get(const pair<Node, Node> &key);

	/**
	 * Get a Harc without constructing it if it doesn't exist.
	 *     If the Harc is found its pointer is put into result and this
	 *     function returns true. Otherwise nullptr is put into result and
	 *     the function returns false. Used internally for paths over virtual
	 *     harcs.
	 * @param result Reference to a Harc pointer filled with result.
	 * @return True if found, false otherwise.
	 */
	bool get(const pair<Node, Node> &key, Harc*& result);

	bool get(const Node &a, const Node &b, Harc*& result) {
		return get((a < b) ? pair<Node,
			Node>(a, b) : pair<Node, Node>(b, a),
			result);
	}

	/**
	 * Evaluate a normalised path through the fabric. If a dependant Harc is
	 * given then it will be added as a dependant to all visited Harcs in the
	 * path.
	 * @param p Normalised nested path through fabric.
	 * @param dep The Harc to add as dependant on this path.
	 * @return Result of following the normalised path p.
	 */
	Node path(const vector<Node> &p, const Harc *dep = nullptr);

	/**
	 * Evaluate several simple paths in parallel. If dep is given then it is
	 * added as a dependant on each of the paths.
	 * @param p Set of simple paths to evaluate separately.
	 * @param res Vector to put each result into.
	 * @param dep Harc to add as dependant on each path.
	 */
	vector<Node> paths(
		const vector<vector<Node>> &p,
		const Harc *dep = nullptr);

	/**
	 * Get the fabric singleton.
	 */
	static Fabric &singleton();

	/**
	 * Number of ticks since program start. Used to record when a relation
	 * was last accessed or changed.
	 */
	static unsigned long long counter() { return counter__; }

	/**
	 * Number of milliseconds per tick.
	 */
	constexpr static unsigned long long counterResolution() { return 100; }

	// constexpr static int sig_prop_max() { return 20; }

	private:
	Node path_s(const vector<Node> &, const Harc *dep = nullptr);
	static bool path_r(
			const vector<vector<Node>> &p,
			Node *res,
			int s, int e,
			const Harc *dep);
	void logChange(const Harc *h);
	static void counterThread();

	void updatePartners(const Node &n, list<Harc*>::iterator &it);
	void reposition(const list<Harc*> &p, list<Harc*>::iterator &it);
	void add(Harc *h);

	unordered_map<pair<Node, Node>, Harc*, TailHash>  harcs_;
	unique_ptr<forward_list<const Harc*>>             changes_;
	unordered_map<Node, list<Harc*>, NidHash>         partners_;

	static std::atomic<unsigned long long> counter__;
};

extern Fabric fabric;

inline auto begin(const Node &n) {
	return fabric.partners(n).begin();
}

inline auto end(const Node &n) {
	return fabric.partners(n).end();
}

};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

