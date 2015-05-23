/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_HARC_H_
#define FDSB_HARC_H_

#include <list>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <utility>
#include <chrono>
#include <ostream>

#include "fdsb/nid.hpp"
#include "fdsb/definition.hpp"

using std::pair;
using std::list;
using std::vector;
using std::chrono::time_point;
using std::chrono::system_clock;

namespace fdsb {

class Fabric;

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc {
	friend class Fabric;

	public:
	enum struct Flag : unsigned int {
		none = 0x00,
		log = 0x01,			/**< Record changes to this Harc */
		meta = 0x02,		/**< This Harc has meta-data */
		defined = 0x04,		/**< This Harc has a non-constant definition */
	};

	/**
	 * Get the head of this hyper-arc. Evaluate the definition if it is
	 * out-of-date. Also causes activation to change and a possible update of
	 * all partner Harcs, possibly involving a resort of partners.
	 * @return Head node of Harc
	 */
	const Nid &query();

	const Nid &query() const;

	/**
	 * Get the pair of tail nodes that uniquely identify this Harc.
	 * @return Tail node pair.
	 */
	inline const pair<Nid, Nid> &tail() const;

	/**
	 * Does the tail of this Harc contain the given node?
	 * @param n The node id to check for.
	 * @return True if the node is part of the tail.
	 */
	inline bool tail_contains(const Nid &n) const;

	/**
	 * What is the other node in this Harcs tail?
	 * @param n The unwanted tail node.
	 * @return The other tail node, not n.
	 */
	inline const Nid &tail_partner(const Nid &n) const;

	/**
	 * Define the Harc as having a constant head node. If there is an
	 * existing non-constant definition, it is removed.
	 * @param d Node the Harc points to.
	 */
	void define(const Nid &);

	/**
	 * Define the Harc as having a normalised path definition to work out
	 * its head node.
	 */
	void define(const vector<vector<Nid>> &d);

	void define(Definition *def);

	inline const Definition *definition() const;

	inline void set_flag(Flag f);
	inline bool check_flag(Flag f) const;
	inline void clear_flag(Flag f);

	/**
	 * Return a list of Harcs that are dependant upon this Harc. Note that this
	 * will rarely be an exhaustive list of all dependencies because those Harcs
	 * only become dependent upon this Harc when they are first evaluated.
	 */
	inline const list<const Harc*> &dependants() const;

	/**
	 * Calculate the significance value between 0.0 and 1.0 of this hyper-arc.
	 * @return Signficance of Harc between 0.0 and 1.0.
	 */
	float significance() const;

	/**
	 * Time in seconds since this Harc was last queried.
	 * @return Seconds since last query.
	 */
	float last_query() const;

	Harc &operator[](const Nid &);
	Harc &operator=(const Nid &);
	bool operator==(const Nid &);

	private:
	pair<Nid, Nid> m_tail;
	union {
	Nid m_head;
	Definition *m_def;
	};
	Flag m_flags;
	unsigned long long m_lastquery;
	float m_strength;
	mutable list<const Harc*> m_dependants;

	// Might be moved to meta structure
	list<Harc*>::iterator m_partix[2];

	Harc() {}  							/* Only Fabric should call */
	explicit Harc(const pair<Nid, Nid> &t);

	void dirty() const;  				/* Mark as out-of-date and propagate */
	void add_dependant(const Harc &);  	/* Notify given Harc on change. */
	void update_partners(const Nid &n, list<Harc*>::iterator &it);
	void reposition_harc(const list<Harc*> &p, list<Harc*>::iterator &it);
};

/* ==== Relational Operators ================================================ */

constexpr Harc::Flag operator | (Harc::Flag lhs, Harc::Flag rhs) {
	return (Harc::Flag)(static_cast<unsigned int>(lhs)
			| static_cast<unsigned int>(rhs));
}

inline Harc::Flag &operator |= (Harc::Flag &lhs, Harc::Flag rhs) {
	lhs = lhs | rhs;
	return lhs;
}

constexpr Harc::Flag operator & (Harc::Flag lhs, Harc::Flag rhs) {
	return (Harc::Flag)(static_cast<unsigned int>(lhs)
			& static_cast<unsigned int>(rhs));
}

inline Harc::Flag &operator &= (Harc::Flag &lhs, Harc::Flag rhs) {
	lhs = lhs & rhs;
	return lhs;
}

constexpr Harc::Flag operator ~(Harc::Flag f) {
	return (Harc::Flag)(~static_cast<unsigned int>(f));
}


/* ==== Inline Implementations ============================================== */

inline void Harc::set_flag(Flag f) { m_flags |= f; }
inline bool Harc::check_flag(Flag f) const { return (m_flags & f) == f; }
inline void Harc::clear_flag(Flag f) { m_flags &= ~f; }

std::ostream &operator<<(std::ostream &os, const Harc &h);

inline const pair<Nid, Nid> &Harc::tail() const { return m_tail; }

inline bool Harc::tail_contains(const Nid &n) const {
	return (m_tail.first == n) || (m_tail.second == n);
}

inline const Nid &Harc::tail_partner(const Nid &n) const {
	return (m_tail.first == n) ? m_tail.second : m_tail.first;
}

inline const list<const Harc*> &Harc::dependants() const {
	return m_dependants;
}

inline const Definition *Harc::definition() const {
	return (check_flag(Flag::defined)) ? m_def : nullptr;
}

};  // namespace fdsb

#endif /* FDSB_HARC_H_ */
