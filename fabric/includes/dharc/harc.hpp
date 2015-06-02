/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_HARC_HPP_
#define DHARC_HARC_HPP_

#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <utility>
#include <chrono>
#include <iostream>
#include <string>
#include <algorithm>

#include "dharc/node.hpp"
#include "dharc/tail.hpp"
#include "dharc/lock.hpp"
#include "dharc/definition.hpp"
#include "dharc/significance.hpp"

using std::pair;
using std::multimap;
using std::list;
using std::vector;
using std::chrono::time_point;
using std::chrono::system_clock;

using dharc::Tail;

namespace dharc {

class Fabric;

namespace fabric {

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc {
	friend class dharc::Fabric;

	public:
	enum struct Flag : uint64_t {
		none =       0b00000000,
		log =        0b00000001,
		meta =       0b00000010,
		defined =    0b00000100,
		outofdate =  0b00001000,
		abstract =   0b00010000,
	};

	/**
	 * Get the head of this hyper-arc. Evaluate the definition if it is
	 * out-of-date. Also causes activation to change and a possible update of
	 * all partner Harcs, possibly involving a resort of partners.
	 * @return Head node of Harc
	 */
	const Node &query() const;

	/**
	 * Get the pair of tail nodes that uniquely identify this Harc.
	 * @return Tail node pair.
	 */
	inline const Tail &tail() const;

	/**
	 * Does the tail of this Harc contain the given node?
	 * @param n The node id to check for.
	 * @return True if the node is part of the tail.
	 */
	inline bool tailContains(const Node &n) const;

	/**
	 * What is the other node in this Harcs tail?
	 * @param n The unwanted tail node.
	 * @return The other tail node, not n.
	 */
	inline const Node &tailPartner(const Node &n) const;

	inline bool checkFlag(Flag f) const;

	inline void startRecording() const { setFlag(Flag::log); }
	inline void stopRecording() const { clearFlag(Flag::log); }

	/**
	 * Define the Harc as having a constant head node. If there is an
	 * existing non-constant definition, it is removed.
	 * @param d Node the Harc points to.
	 */
	void define(const Node &);

	/**
	 * Define the Harc as having a normalised path definition to work out
	 * its head node.
	 */
	void define(const vector<vector<Node>> &definition);

	std::string definition() const;

	void definition(std::ostream &os) const;

	/**
	 * Return a list of Harcs that are dependant upon this Harc. Note that this
	 * will rarely be an exhaustive list of all dependencies because those Harcs
	 * only become dependent upon this Harc when they are first evaluated.
	 */
	inline const list<const Harc*> *dependants() const;

	/**
	 * Calculate the significance value between 0.0 and 1.0 of this hyper-arc.
	 * @return Signficance of Harc between 0.0 and 1.0.
	 */
	inline float significance() const { return sig_.significance(); }

	inline float changeSignificance(float prev) const {
		return sig_.change(prev);
	}

	inline float partnerSignificance(float prev) const {
		return sig_.partner(prev);
	}

	/**
	 * Time in seconds since this Harc was last queried.
	 * @return Seconds since last query.
	 */
	inline float lastQuery() const { return sig_.lastBoost(); }

	private:
	HarcMap::const_iterator        tail_;
	mutable Node                   head_;
	mutable std::atomic<uint64_t>  flags_;
	Definition*                    def_;

	mutable Significance           sig_;
	mutable dharc::Lock            lock_;

	// Might be moved to meta structure
	mutable list<const Harc*>*               dependants_;
	// mutable multimap<float, Node>::iterator  partix_[2];



	inline void setFlag(Flag f) const;
	inline void clearFlag(Flag f) const;

	Harc();
	void setIterator(const HarcMap::iterator &it) { tail_ = it; }
	void dirty() const;  				/* Mark as out-of-date and propagate */
	void addDependant(const Harc &);  	/* Notify given Harc on change. */

	Harc *instantiate(const Node &any);
};


/* ==== Inline Implementations ============================================== */

inline void Harc::setFlag(Flag f) const {
	flags_ |= static_cast<unsigned char>(f);
}

inline bool Harc::checkFlag(Flag f) const {
	return (flags_ & static_cast<unsigned char>(f));
}

inline void Harc::clearFlag(Flag f) const {
	flags_ &= ~static_cast<unsigned char>(f);
}

std::ostream &operator<<(std::ostream &os, const Harc &h);

inline const Tail &Harc::tail() const { return (*tail_).first; }

inline bool Harc::tailContains(const Node &n) const {
	return std::binary_search(tail().begin(), tail().end(), n);
}

/*inline const Node &Harc::tailPartner(const Node &n) const {
	return (tail_.first == n) ? tail_.second : tail_.first;
}
*/

inline const list<const Harc*> *Harc::dependants() const {
	return dependants_;
}


};  // namespace fabric
};  // namespace dharc

#endif  // DHARC_HARC_HPP_

