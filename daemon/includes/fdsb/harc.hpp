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

#include "fdsb/nid.hpp"

using std::pair;

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
		log = 0x01,
		meta = 0x02,
		defined = 0x04,
	};

	/**
	 * Get the head of this hyper-arc. Evaluate the definition if it is
	 * out-of-date.
	 */
	const Nid &query();

	const pair<Nid, Nid> &tail() { return m_tail; }

	bool tail_has(const Nid &n) {
		return (m_tail.first == n) || (m_tail.second == n);
	}
	
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
	
	float significance() const { return m_sig; };

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
	float m_sig;
	std::list<Harc*> m_dependants;  	/* Who depends upon me */

	Harc() {}  							/* Only Fabric should call */
	Harc(const pair<Nid, Nid> &t);
	void dirty();  						/* Mark as out-of-date and propagate */
	void add_dependant(Harc &);  		/* Notify given Harc on change. */
	void update_partners(const Nid &n);
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

};  // namespace fdsb

#endif /* FDSB_HARC_H_ */
