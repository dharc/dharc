#ifndef _FDSB_NID_
#define _FDSB_NID_

namespace fdsb
{

enum struct NidType : unsigned char
{
	special,
	integer,
	real,
	character,
	custom,
};

struct Nid
{
	Nid(NidType t, unsigned long long v)
			: t(t), v(v) {}

	NidType t;
	unsigned long long v;
};

Nid operator"" _nid(long long v)
{
	return Nid(NidType::integer, (unsigned long long)v);
}
};

#endif