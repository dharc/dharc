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
#include <map>

#include "dharc/node.hpp"
#include "dharc/tail.hpp"
#include "dharc/harc.hpp"
// #include "dharc/lifobuffer.hpp"

using std::forward_list;
using std::vector;
using std::unordered_map;
using std::chrono::time_point;
using std::pair;
using std::list;
using std::multimap;
using std::size_t;

using dharc::fabric::Harc;
// using dharc::LIFOBuffer;

namespace dharc {

/**
 * The Dharc Fabric: a dynamic hypergraph.
 *     Stores all the nodes and hyperarcs relating the nodes together. Each
 *     hyperarc (harc) has metrics about its significance and activation to
 *     improve pattern searching and suggestion. A harc can also be given a
 *     definition that frames it in-terms of other nodes and harcs. Low-level
 *     search and access functions are provided, usually to significance sorted
 *     data, that can then be manipulated by higher-level pattern matching and
 *     search algorithms. This class hides any storage concerns and is
 *     threadsafe. The class is entirely static, only one fabric is possible
 *     per instance of the fabric server.
 */
class Fabric {
	friend dharc::fabric::Harc;

	public:
	Fabric()=delete;

	static void initialise();
	static void finalise();



	/**
	 * Fill a vector with a given number of the most significant recent changes.
	 *     The changes returned are sorted by activation significance which is
	 *     a combination of how significant the change is and how recently it
	 *     it was changed.  Use maxChanges() to get the maximum number of
	 *     available changes. If more changes are requested than are available
	 *     then only the available number are put into the vector.
	 * @param vec Vector to fill with requested changes.
	 * @param count How many changes to request.
	 */
	static void changes(vector<Tail>& vec, size_t count);



	/**
	 * Maximum number of harc changes in the change log.
	 * @return Number of changes available.
	 */
	static size_t maxChanges();



	/**
	 * Fill a vector with 'count' number of partner nodes.
	 *     A partner node is a node involved in some tail of a hyperarc with
	 *     a given node. This function fills a vector the the requested number
	 *     of the most significant partner nodes, by default starting at the
	 *     most significant unless 'start' is given. If the requested number of
	 *     of nodes is greater than those available then the vector is resized
	 *     to the number available.
	 * @param node The node to get partners of.
	 * @param vec Vector to fill with results.
	 * @param count Number of partners to return into the vector.
	 * @param start Offset to this position in overal partners list.
	 */
	static void partners(	const Node&   node,
							vector<Node>& vec,
							size_t        count,
							size_t        start = 0);



	/**
	 * Query the head node of a hyperarc.
	 *     Given the tail nodes of a hyperarc, lookup the hyperarc and ask for
	 *     its head node. This may involve the evaluation of the hyperarcs
	 *     definition if it is out-of-date. If the requested hyperarc does not
	 *     exist then it is created and the head node null is returned. A query
	 *     will also activate a hyperarch and modify is significance and
	 *     activation status.
	 * @param tail Pair of tail nodes to identify the hyperarc.
	 * @return Head node of hyperarch.
	 */
	static Node query(const Tail &tail);



	/**
	 * Give a harc a constant head node.
	 *     Removes any existing definition and sets the head of the harc to
	 *     the given node. This will cause an activation of the harc and will
	 *     log this harc as having changed if the harcs log flag is set. If the
	 *     harc does not exist, it is created.
	 * @see define
	 * @param tail A set of tail nodes to identify the harcs.
	 * @param head The new head node for the harc.
	 */
	static void set(const Tail &tail, const Node &head) { define(tail, head); }



	/**
	 * Give a harc a constant head node.
	 * @see set
	 */
	static void define(const Tail &tail, const Node &head);



	/**
	 * Give the harc a path definition instead of a constant head.
	 *     Will replace any existing definition and any constant head node.
	 *     The definition will only be evaluated is the harc is queried for the
	 *     head node. If any path used by the definition after it is evaluated
	 *     changes then the harc is marked out-of-date and will be re-evaluated
	 *     upon the next query. If the harc does not exist it is created.
	 * @param tail Tail nodes to uniquely idenfify the harc.
	 * @param def Path for the definition the harc should have.
	 */
	static void define(const Tail &tail, const vector<vector<Node>> &def);

	static Node unique();

	static void unique(int count, Node &first, Node &last);


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


	static unordered_map<Tail, Harc*, TailHash>                 harcs__;
	static multimap<float, const Harc*>                         changes__;
	static unordered_map<Node, multimap<float, Node>, NidHash>  partners__;

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

	static void updatePartners(const Harc *h);
	
	static void add(Harc *h);
};

/*inline auto begin(const Node &n) {
	return Fabric::partners(n).begin();
}

inline auto end(const Node &n) {
	return Fabric::partners(n).end();
}*/

};  // namespace dharc

#endif  // DHARC_FABRIC_HPP_

