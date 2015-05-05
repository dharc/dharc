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

class Definition;

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc {
	struct Definition {
		explicit Definition(const std::vector<std::vector<Nid>> &d) :
			outofdate(true),
			lock(ATOMIC_FLAG_INIT),
			def(d) {}

		bool outofdate;
		std::atomic_flag lock;
		std::vector<std::vector<Nid>> def;
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

	void define(const std::vector<std::vector<Nid>> &);

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

	/**
	 * Find or create a hyper-arc with the given tail nodes.
	 */
	static Harc &get(const Nid &, const Nid &);

	/**
	 * Navigate a simple path and return resulting node.
	 */
	static Nid path(const std::vector<Nid> &, Harc *dep = nullptr);

	/**
	 * Navigate a path of paths and return combined result. Each sub path
	 * is explored in parallel before the results are then combined as a path
	 * and the final result returned.
	 */
	static Nid path(const std::vector<std::vector<Nid>> &, Harc *dep = nullptr);

	private:
	Nid m_tail[2];
	Nid m_head;
	Definition *m_def;
	std::list<Harc*> m_dependants;

	/* Prevent empty harc */
	Harc() {}
	Harc(const Nid &, const Nid &);
	void dirty();
	void add_dependant(Harc &);

	static std::unordered_multimap<unsigned long long, Harc*> s_fabric;
};

};  // namespace fdsb

#endif /* FDSB_HARC_H_ */
