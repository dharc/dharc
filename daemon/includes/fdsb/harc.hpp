#ifndef _FDSB_HARC_H_
#define _FDSB_HARC_H_

#include "fdsb/nid.hpp"
#include <set>
#include <vector>
#include <unordered_map>

namespace fdsb
{
	
class Definition;

/**
 * Hyper-arc class to represent relations between 3 nodes.
 * Each Harc can be given a definition and can be made dependant upon other
 * Harc's so that they update when others are changed.
 */
class Harc
{
public:
	const Nid &query() const { return m_head; }
	
	/**
	 * Define the Harc as having a fixed head node.
	 */
	void define(const Nid &);
	
	void define(const std::vector<std::vector<Nid>> &);
	
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
	
	static Harc &get(const Nid &, const Nid &);
	static Nid path(const std::vector<Nid> &, Harc *dep=nullptr);
	static Nid path(const std::vector<std::vector<Nid>> &, Harc *dep=nullptr);
	
	/**
	 * Perform a single coalescence step. Each step obtains new values for
	 * out-of-date Harcs and then, together, writes those changes to Harcs.
	 * If there are cyclical dependencies then each call to coalesce() will
	 * cause changes even if no external changes have been made. Therefore,
	 * in dynamic scenarios coalesce() should be called repeatedly. It returns
	 * the number of changes that have been made, or 0 if no changes.
	 */
	static int coalesce();
	
private:
	Nid m_tail[2];
	Nid m_head;
	std::vector<std::vector<Nid>> m_def;
	std::set<Harc*> m_dependants;
	
	/* Prevent empty harc */
	Harc() {}
	Harc(const Nid &, const Nid &);
	void dirty();
	void add_dependant(Harc &);
	
	static std::unordered_multimap<unsigned long long,Harc*> s_fabric;
	static std::set<Harc*> s_dirty;
	static std::unordered_map<Harc*,Nid> s_changes;
	
	static void process_dirty();
	static void write_changes();
};

};

#endif /* _FDSB_HARC_H_ */
