/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_DEFINITION_H_
#define FDSB_DEFINITION_H_

#include <string>
#include <vector>
#include <atomic>

#include "dharc/nid.hpp"

using std::vector;
using std::string;
using std::atomic_flag;

namespace dharc {
class Harc;

/**
 * Represent a Harc definition and provide a means of converting it to/from
 * various forms as well as doing any internal optimisations for frequently
 * evaluated definitions. Each definition uses a lazy evaluation approach.
 */
class Definition {
	public:
	/**
	 * Evaluate the definition path and return the result. It may return only
	 * a cached result rather than actually performing an expensive evaluation.
	 * @param h The hyperarc that this definition is evaluated for.
	 * @return Result of following definition path.
	 */
	const Nid &evaluate(const Harc *h) const;

	/**
	 * [internal] Mark this definition as out-of-date and therefore in need of
	 * re-evaluation when evaluate() is next called.
	 */
	inline void mark() const;

	/**
	 * Convert this definition to a standard string representation. Passing the
	 * resulting string to from_string is guaranteed to reproduce the
	 * definition exactly.
	 * @return A string representing this definition.
	 */
	string to_string() const;

	/**
	 * Dump the definition as a raw normalised path.
	 * @return An array of arrays of Nids.
	 */
	vector<vector<Nid>> to_path() const;

	static Definition *from_string(const string &str);
	static Definition *from_path(const vector<vector<Nid>> &path);

	private:
	mutable bool m_outofdate;		/* Is this definition out-of-date. */
	mutable atomic_flag m_lock;		/* Threading lock for cache etc... */
	vector<vector<Nid>> m_path;		/* Actual path for definition */
	mutable Nid m_cache;			/* Last calculated value of definition */

	Definition() :
		m_outofdate(true),
		m_lock(ATOMIC_FLAG_INIT),
		m_cache(null_n) {}

	friend std::ostream &operator<<(std::ostream &os, const Definition &d);
};

/* ==== Inline Implementations ============================================== */

inline void Definition::mark() const {
	m_outofdate = true;
}

std::ostream &operator<<(std::ostream &os, const Definition &d);

};  // namespace dharc

#endif  /* FDSB_DEFINITION_H_ */
