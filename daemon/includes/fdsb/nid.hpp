#ifndef _FDSB_NID_
#define _FDSB_NID_

namespace fdsb
{

/**
 * Node Identifier Plain Old Data type.
 */
struct Nid
{
	enum struct Type : unsigned char
	{
		special,
		integer,
		real,
		character,
		allocated,
	};
	
	enum struct Special
	{
		null,
		bool_true,
		bool_false,
	};

	Type t;
	union
	{
	unsigned long long i;
	Special s;
	long double d;
	char c;
	};
	
	/** Generate a new unique node id. */
	static Nid unique();
};


constexpr Nid operator"" _nid(unsigned long long v)
{
	return Nid{Nid::Type::integer, { .i = v }};
}

constexpr Nid operator"" _nid(long double v)
{
	return Nid{Nid::Type::real, { .d = v }};
}

constexpr Nid operator"" _nid(char v)
{
	return Nid{Nid::Type::character, { .c = v }};
}

constexpr Nid null_nid = {Nid::Type::special,{ .s = Nid::Special::null }};
constexpr Nid true_nid = {Nid::Type::special,{ .s = Nid::Special::bool_true }};
constexpr Nid false_nid = {Nid::Type::special,{ .s = Nid::Special::bool_false }};

};

#endif
