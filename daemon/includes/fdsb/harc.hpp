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

using std::pair;
using std::list;
using std::chrono::time_point;
using std::chrono::system_clock;

namespace fdsb {

class Fabric;
typedef std::vector<std::vector<Nid>> Path;

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc {
	friend class Fabric;

	struct Definition {
		explicit Definition(const fdsb::Path &d) :
			outofdate(true),
			lock(ATOMIC_FLAG_INIT),
			def(d) {}

		bool outofdate;
		std::atomic_flag lock;
		fdsb::Path def;
		Nid cache;
	};

	public:
	enum struct Flag : unsigned int {
		none = 0x00,
		log = 0x01,			/** Record changes to this Harc */
		meta = 0x02,		/** This Harc has meta-data */
		defined = 0x04,		/** This Harc has a non-constant definition */
	};

	/**
	 * Get the head of this hyper-arc. Evaluate the definition if it is
	 * out-of-date. Also causes activation to change and a possible update of
	 * all partner Harcs, possibly involving a resort of partners.
	 * @return Head node of Harc
	 */
	const Nid &query();

	/**
	 * Get the pair of tail nodes that uniquely identify this Harc.
	 * @return Tail node pair.
	 */
	const pair<Nid, Nid> &tail() { return m_tail; }

	/**
	 * Does the tail of this Harc contain the given node?
	 * @param n The node id to check for.
	 * @return True if the node is part of the tail.
	 */
	bool tail_has(const Nid &n) {
		return (m_tail.first == n) || (m_tail.second == n);
	}

	/**
	 * What is the other node in this Harcs tail?
	 * @param n The unwanted tail node.
	 * @return The other tail node, not n.
	 */
	const Nid &tail_other(const Nid &n) {
		return (m_tail.first == n) ? m_tail.second : m_tail.first;
	}

	/**
	 * Define the Harc as having a fixed head node.
	 */
	void define(const Nid &);

	void define(const fdsb::Path &);

	inline void set_flag(Flag f);
	inline bool check_flag(Flag f) const;
	inline void clear_flag(Flag f);

	bool is_out_of_date() const {
		if (check_flag(Flag::defined)) {
			return m_def->outofdate;
		} else {
			return false;
		}
	}

	/**
	 * Each time this is called the significance is reduced before being
	 * returned. It is boosted by querying the Harc.
	 */
	float significance();

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
	std::list<Harc*> m_dependants;  	/* Who depends upon me */

	// Might be moved to meta structure
	std::list<Harc*>::iterator m_partix[2];

	Harc() {}  							/* Only Fabric should call */
	explicit Harc(const pair<Nid, Nid> &t);
	void dirty();  						/* Mark as out-of-date and propagate */
	void add_dependant(Harc &);  		/* Notify given Harc on change. */
	void update_partners(const Nid &n, std::list<Harc*>::iterator &it);
	void reposition_harc(const list<Harc*> &p, std::list<Harc*>::iterator &it);
};

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

inline void Harc::set_flag(Flag f) { m_flags |= f; }
inline bool Harc::check_flag(Flag f) const { return (m_flags & f) == f; }
inline void Harc::clear_flag(Flag f) { m_flags &= ~f; }

std::ostream &operator<<(std::ostream &os, Harc &h);

};  // namespace fdsb

#endif /* FDSB_HARC_H_ */
