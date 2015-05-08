/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_HARC_H_
#define FDSB_HARC_H_

#include <list>
#include <vector>
#include <unordered_map>
#include <atomic>

#include "fdsb/nid.hpp"

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
	};

	public:
	/**
	 * Get the head of this hyper-arc. Evaluate the definition if it is
	 * out-of-date.
	 */
	const Nid &query();

	/**
	 * Define the Harc as having a fixed head node.
	 */
	void define(const Nid &);

	void define(const fdsb::Path &);

	bool is_out_of_date() const {
		if (m_def) {
			return m_def->outofdate;
		} else {
			return false;
		}
	}

	/**
	 * Compare this Harcs tail with a pair of Nids. Order does not matter.
	 */
	bool equal_tail(const Nid &a, const Nid &b) const {
		return 	(m_tail[0] == a && m_tail[1] == b) ||
				(m_tail[0] == b && m_tail[1] == a);
	}

	/**
	 * Get the tail nodes. There are only 2 so only integers 0 and 1 can be
	 * used in the template parameter.
	 */
	template <int I>
	const Nid &tail() const {
		static_assert(I < 2 && I >= 0, "Tail only has 2 nodes.");
		return m_tail[I];
	}

	Harc &operator[](const Nid &);
	Harc &operator=(const Nid &);
	bool operator==(const Nid &);

	private:
	Nid m_tail[2];
	Nid m_head;
	Definition *m_def;
	std::list<Harc*> m_dependants;  	/* Who depends upon me */

	Harc() {}  							/* Prevent empty harc */
	Harc(const Nid &, const Nid &);  	/* Only Fabric should call this */
	void dirty();  						/* Mark as out-of-date and propagate */
	void add_dependant(Harc &);  		/* Notify given Harc on change. */
};

};  // namespace fdsb

#endif /* FDSB_HARC_H_ */
