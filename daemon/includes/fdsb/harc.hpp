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
	const Nid &tail(int i) { return m_tail[i]; }
	bool is_out_of_date() { return m_out_of_date; }
	
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
