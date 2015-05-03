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
	const Nid &tail(int i) { return tail[i]; }
	
	Harc &operator[](const Nid &);
	Harc &operator=(const Nid &);
	
	private:
	Nid tail[2];
	Nid head;
	//Definition;
	bool out_of_date;
	std::list<Harc*> dependants;
};

};

#endif /* _FDSB_HARC_H_ */
