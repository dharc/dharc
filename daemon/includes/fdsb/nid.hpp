#ifndef _FDSB_NID_
#define _FDSB_NID_

namespace fdsb
{
	
class Harc;

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
	
	/**
	 * Generate a new unique node id.
	 */
	static Nid unique();
	
	/**
	 * Generate a combined hash value from two NIDs.
	 * Order is not important.
	 */
	constexpr static unsigned long long dual_hash(const Nid &a, const Nid &b)
	{
		return a.i ^ b.i;
	}
	
	/**
	 * Combine with a second NID to retrieve a Harc.
	 * NOTE: Implemented as part of fabric(.cpp).
	 */
	Harc &operator[](const Nid &);
};

/**
 * Integer NID literals.
 * e.g. Nid x = 1234_nid;
 */
constexpr Nid operator"" _n(unsigned long long v)
{
	return Nid{Nid::Type::integer, { .i = v }};
}

/**
 * Real NID literals.
 * e.g. Nid x = 12.34_nid;
 */
constexpr Nid operator"" _n(long double v)
{
	return Nid{Nid::Type::real, { .d = v }};
}

/**
 * Character NID literals.
 * e.g. Nid x = 'a'_nid;
 */
constexpr Nid operator"" _n(char v)
{
	return Nid{Nid::Type::character, { .c = v }};
}

constexpr Nid null_n = {Nid::Type::special,{ .s = Nid::Special::null }};
constexpr Nid true_n = {Nid::Type::special,{ .s = Nid::Special::bool_true }};
constexpr Nid false_n = {Nid::Type::special,{ .s = Nid::Special::bool_false }};

/**
 * NID equality comparison.
 */
constexpr bool operator==(const Nid &a, const Nid &b)
{
	return a.t == b.t && a.i == b.i;
}

/**
 * NID inequality comparison.
 */
constexpr bool operator!=(const Nid &a, const Nid &b)
{
	return a.t != b.t || a.i != b.i;
}

};

#endif
