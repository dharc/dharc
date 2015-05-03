#ifndef _FDSB_FABRIC_H_
#define _FDSB_FABRIC_H_

namespace fdsb
{
	
class Harc;
struct Nid;

Harc &get(const Nid &, const Nid &);
void add(Harc &);
void add(const Nid &, const Nid &);

};

#endif /* _FDSB_FABRIC_H_ */
