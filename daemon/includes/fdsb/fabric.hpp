#ifndef _FDSB_FABRIC_H_
#define _FDSB_FABRIC_H_

#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"

namespace fdsb
{

inline Harc &get(const Nid &a, const Nid &b) { return Harc::get(a,b); }
inline Nid path(const std::vector<Nid> &p) { return Harc::path(p,nullptr); }
inline Nid path(const std::vector<std::vector<Nid>> &p) { return Harc::path(p,nullptr); }

};

#endif /* _FDSB_FABRIC_H_ */
