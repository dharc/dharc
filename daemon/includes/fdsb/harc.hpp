#ifndef _FDSB_HARC_H_
#define _FDSB_HARC_H_

#include "fdsb/nid.hpp"
#include <list>

namespace fdsb
{

class Harc
{
	public:
	Harc(const Nid &, const Nid &);
	const Nid &query();
	void add_dependant(Harc &);
	void mark();
	void define(const Nid &);
	bool is_out_of_date() { return m_out_of_date; }
	bool equal_tail(const Nid &a, const Nid &b)
	{
		return (m_tail[0] == a && m_tail[1] == b) || (m_tail[0] == b && m_tail[1] == a);
	}
	
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
};

};

#endif /* _FDSB_HARC_H_ */
