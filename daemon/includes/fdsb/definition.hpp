/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef FDSB_DEFINITION_H_
#define FDSB_DEFINITION_H_

#include <string>
#include <vector>
#include <atomic>

#include "fdsb/nid.hpp"

using std::vector;
using std::string;
using std::atomic_flag;

namespace fdsb {
class Harc;

/**
 * Represent a Harc definition and provide a means of converting it to/from
 * various forms as well as doing any internal optimisations for frequently
 * evaluated definitions.
 */
class Definition {
	public:
	const Nid &evaluate(Harc *h);
	inline void mark();
	inline bool is_out_of_date() const;

	string to_string() const;
	vector<vector<Nid>> to_path() const;

	static Definition *from_string(const string &str);
	static Definition *from_path(const vector<vector<Nid>> &path);

	private:
	bool m_outofdate;
	atomic_flag m_lock;
	vector<vector<Nid>> m_path;
	Nid m_cache;

	Definition() :
		m_outofdate(true),
		m_lock(ATOMIC_FLAG_INIT),
		m_cache(null_n) {}
};

/* ==== Inline Implementations ============================================== */

inline void Definition::mark() {
	m_outofdate = true;
}

inline bool Definition::is_out_of_date() const {
	return m_outofdate;
}

};  // namespace fdsb

#endif  /* FDSB_DEFINITION_H_ */
