#ifndef _FDSB_HARC_H_
#define _FDSB_HARC_H_

#include "fdsb/nid.hpp"
#include <list>

namespace fdsb
{

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc
{
public:
	/**
	 * A Harc must be constructed with explicit nodes.
	 */
	Harc(const Nid &, const Nid &);
	
	/**
	 * Queries either the cached head node or calculates a new head node.
	 */
	const Nid &query();
	
	/**
	 * Add another Harc as being dependant upon this one.
	 */
	void add_dependant(Harc &);
	
	/**
	 * Mark this Harc as out-of-date. Also mark all dependant Harc's as
	 * out-of-date.
	 */
	void mark();
	
	/**
	 * Define the Harc as having a fixed head node.
	 */
	void define(const Nid &);
	
	/**
	 * Is this Harc out-of-date?
	 */
	bool is_out_of_date() const { return m_out_of_date; }
	
	/**
	 * Compare this Harcs tail with a pair of Nids. Order does not matter.
	 */
	bool equal_tail(const Nid &a, const Nid &b)
	{
		return (m_tail[0] == a && m_tail[1] == b) || (m_tail[0] == b && m_tail[1] == a);
	}
	
	/**
	 * Get the tail nodes. There are only 2 so only integers 0 and 1 can be
	 * used in the template parameter.
	 */
	template <int I>
	const Nid &tail()
	{
		static_assert(I < 2 && I >= 0, "Tail only has 2 nodes.");
		return m_tail[I];
	}

	
	Harc &operator[](const Nid &);
	Harc &operator=(const Nid &);
	bool operator==(const Nid &);
	
private:
	Nid m_tail[2];
	Nid m_head;
	//Definition;
	bool m_out_of_date;
	std::list<Harc*> m_dependants;
	std::list<Harc*> m_dependencies;
	
	/* Prevent empty harc */
	Harc() {}
};

};

#endif /* _FDSB_HARC_H_ */
