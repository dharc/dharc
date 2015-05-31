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
#include "dharc/tail.hpp"
#include "dharc/harc.hpp"

using std::forward_list;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::chrono::time_point;
using std::pair;
using std::list;
using std::size_t;

using dharc::fabric::Harc;

namespace dharc {

struct TailHash {
	public:
	constexpr size_t operator()(const Tail &x) const {
		return x.first.value*3 + x.second.value;
	}
};

struct NidHash {
	public:
	constexpr size_t operator()(const Node &x) const {
		return x.value;
	}
};

class Fabric {
	friend dharc::fabric::Harc;

	public:
	Fabric()=delete;

	static void initialise();
	static void finalise();

	/**
	 * A list of Harc changes since this function was last called. The change
	 * log is reset when this function is called.
	 */
	static unique_ptr<forward_list<const Harc*>> changes();

	/**
	 * Array of all Harcs that the given Node is involved in.
	 * This array is intended to be sorted by significance, but since
	 * significance always changes there is no guarentee that it is up-to-date.
	 */
	static const list<Harc*> &partners(const Node &);



	static Node query(const Tail &tail);

	static void set(const Tail &tail, const Node &head) { define(tail, head); }
	static void define(const Tail &tail, const Node &head);

	static void define(const Tail &tail, const vector<vector<Node>> &def);

	static Node unique();


	/**
	 * Evaluate a normalised path through the fabric. If a dependant Harc is
	 * given then it will be added as a dependant to all visited Harcs in the
	 * path.
	 * @param p Normalised nested path through fabric.
	 * @param dep The Harc to add as dependant on this path.
	 * @return Result of following the normalised path p.
	 */
	static Node path(const vector<Node> &p, const Harc *dep = nullptr);

	/**
	 * Evaluate several simple paths in parallel. If dep is given then it is
	 * added as a dependant on each of the paths.
	 * @param p Set of simple paths to evaluate separately.
	 * @param res Vector to put each result into.
	 * @param dep Harc to add as dependant on each path.
	 */
	static vector<Node> paths(
		const vector<vector<Node>> &p,
		const Harc *dep = nullptr);



	static size_t linkCount()        { return linkcount__; }
	static size_t nodeCount()        { return nodecount__; }
	static size_t definedLinks()     { return variablelinks__; }
	static float  queriesPerSecond() {
		return (static_cast<float>(querycount__) /
				static_cast<float>(counter__)) *
				static_cast<float>(counterResolution());
	}
	static float  changesPerSecond() {
		return (static_cast<float>(changecount__) /
				static_cast<float>(counter__)) *
				static_cast<float>(counterResolution());
	}



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
	static unordered_map<Tail, Harc*, TailHash>       harcs__;
	static unique_ptr<forward_list<const Harc*>>      changes__;
	static unordered_map<Node, list<Harc*>, NidHash>  partners__;

	static std::atomic<size_t> linkcount__;
	static std::atomic<size_t> nodecount__;
	static std::atomic<size_t> variablelinks__;
	static std::atomic<size_t> changecount__;
	static std::atomic<size_t> querycount__;

	static std::atomic<unsigned long long> counter__;



	static Harc &get(const Node &a, const Node &b) {
		return get((a < b) ? pair<Node, Node>(a, b) : pair<Node, Node>(b, a));
	}
	static Harc &get(const Tail &key);
	static bool get(const Tail &key, Harc*& result);
	static bool get(const Node &a, const Node &b, Harc*& result) {
		return get((a < b) ? pair<Node,
			Node>(a, b) : pair<Node, Node>(b, a),
			result);
	}

	static Node path_s(const vector<Node> &, const Harc *dep = nullptr);
	static bool path_r(
			const vector<vector<Node>> &p,
			Node *res,
			int s, int e,
			const Harc *dep);
	static void logChange(const Harc *h);
	static void counterThread();

	static void updatePartners(const Node &n, list<fabric::Harc*>::iterator &it);
	static void reposition(const list<Harc*> &p, list<fabric::Harc*>::iterator &it);
	static void add(Harc *h);
};

inline auto begin(const Node &n) {
	return Fabric::partners(n).begin();
}

inline auto end(const Node &n) {
	return Fabric::partners(n).end();
}

};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

