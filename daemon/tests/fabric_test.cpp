#include "lest.hpp"
#include "dharc/nid.hpp"
#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;

/* ==== MOCKS =============================================================== */

Harc::Harc(const pair<Nid, Nid> &t) :
	m_tail(t),
	m_head(null_n),
	m_flags(Flag::none),
	m_lastquery(Fabric::counter()),
	m_strength(0.0) {}

void Harc::add_dependant(const Harc &h) {
	m_dependants.push_back(&h);
}

const Nid &Harc::query() {
	return m_head;
}

const Nid &Harc::query() const {
	return m_head;
}

void Harc::define(const Nid &n) {
	if (check_flag(Flag::log)) fabric.log_change(this);

	m_head = n;
}

Harc &Harc::operator=(const Nid &n) {
	define(n);
	return *this;
}

bool Harc::operator==(const Nid &n) const {
	return query() == n;
}

std::ostream &dharc::operator<<(std::ostream &os, const Nid &n) {
	os << '[' << static_cast<int>(n.t) << ':' << n.i << ']';
	return os;
}

std::ostream &dharc::operator<<(std::ostream &os, const Harc &h) {
	os << '[' << h.tail().first << ',' << h.tail().second
		<< "->" << h.query() << ']';
	return os;
}

/* ==== END MOCKS =========================================================== */

const lest::test specification[] = {

CASE( "Tails are symetric when looking up a harc" ) {
	fabric.get(10_n,11_n) = 55_n;
	EXPECT( fabric.get(10_n,11_n) == 55_n );
	EXPECT( fabric.get(11_n,10_n) == 55_n );
	fabric.get(11_n,10_n) = 66_n;
	EXPECT( fabric.get(10_n,11_n) == 66_n );
	EXPECT( fabric.get(11_n,10_n) == 66_n );
	
	// Check with different Nid types
	fabric.get('a'_n, 33_n).define('b'_n);
	EXPECT( fabric.get('a'_n, 33_n) == 'b'_n );
	EXPECT( fabric.get(33_n, 'a'_n) == 'b'_n );
},

CASE( "Automatically create an non-existing harc" ) {
	fabric.get(15_n,16_n) = 67_n;
	EXPECT( fabric.get(15_n,16_n) == 67_n );
},

CASE( "Following a single path, all cases" ) {
	fabric.get(1_n, 2_n).define(1000_n);
	fabric.get(1000_n, 3_n).define(1001_n);
	fabric.get(1001_n, 4_n).define(55_n);
	
	// Normal case
	EXPECT( fabric.path({{1_n,2_n,3_n,4_n}}) == 55_n );
	// Degenerate case 1
	EXPECT( fabric.path({{}}) == null_n );
	// Degenerate case 2
	EXPECT( fabric.path({}) == null_n );
	// Base case
	EXPECT( fabric.path({{5_n}}) == 5_n );
},

CASE( "Follow a set of paths in parallel" ) {
	fabric.get(220_n, 2_n).define(2000_n);
	fabric.get(2000_n, 3_n).define(2001_n);
	fabric.get(2001_n, 4_n).define(66_n);
	fabric.get(221_n, 2_n).define(2002_n);
	fabric.get(2002_n, 66_n).define(77_n);
	
	std::vector<Nid> res(10);
	
	// Normal case, 2 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 66_n );
	
	// Normal case, 3 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 77_n );
	EXPECT( res[2] == 66_n );
	
	// Degen case, 3 paths
	fabric.paths({
		{221_n, 2_n, 66_n},
		{},
		{220_n, 2_n, 3_n, 4_n}
	}, res.data());
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == null_n );
	EXPECT( res[2] == 66_n );
},

CASE( "Nested (normalised) path evaluation" ) {
	fabric.get(10_n, 2_n).define(3000_n);
	fabric.get(3000_n, 3_n).define(3001_n);
	fabric.get(3001_n, 4_n).define(66_n);
	fabric.get(11_n, 2_n).define(3002_n);
	fabric.get(3002_n, 66_n).define(77_n);
	
	EXPECT( fabric.path({{11_n,2_n},{10_n,2_n,3_n,4_n}}) == 77_n );
},

CASE( "Bulk evaluation of duplicate paths" ) {
	//Main chain
	fabric.get(2000_n, 2_n).define(5000_n);
	fabric.get(5000_n, 3_n).define(5001_n);
	fabric.get(5001_n, 4_n).define(5002_n);
	fabric.get(5002_n, 5_n).define(5003_n);
	fabric.get(5003_n, 6_n).define(5004_n);
	fabric.get(5004_n, 7_n).define(5005_n);
	fabric.get(5005_n, 8_n).define(10_n);
	
	//Result chain
	fabric.get(10_n, 10_n).define(6000_n);
	fabric.get(6000_n, 10_n).define(6001_n);
	fabric.get(6001_n, 10_n).define(6002_n);
	fabric.get(6002_n, 10_n).define(99_n);
	
	EXPECT( fabric.path({
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n},
		{2000_n, 2_n, 3_n, 4_n, 5_n, 6_n, 7_n, 8_n}
	}) == 99_n );
},

CASE( "Check the change log is filled correctly" ) {
	auto chg = fabric.changes();
	fabric.get(40_n,41_n).set_flag(Harc::Flag::log);
	fabric.get(40_n,42_n).set_flag(Harc::Flag::log);
	fabric.get(40_n,41_n).define(23_n);
	fabric.get(40_n,42_n).define(24_n);
	chg = fabric.changes();
	
	EXPECT ( chg.get() != nullptr );
	if (chg.get()) {
		EXPECT( !chg->empty() );
		if (!chg->empty()) {
			EXPECT( chg->front()->query() == 24_n );
			chg->pop_front();
		}
		EXPECT( !chg->empty() );
		if (!chg->empty()) {
			EXPECT( chg->front()->query() == 23_n );
		}
	}
	
	chg = fabric.changes();
	EXPECT( chg->empty() );
},

CASE( "Check that partners are inserted to partner lists") {
	Nid n1 = 333_n;

	fabric.get(n1, 34_n).define(78_n);
	fabric.get(n1, 35_n).define(79_n);
	fabric.get(n1, 36_n).define(80_n);

	auto partners = fabric.partners(n1);
	EXPECT( static_cast<int>(partners.size()) == 3 );
	
	for (auto i : partners) {
		EXPECT( (i->tail_contains(34_n)
			|| i->tail_contains(35_n)
			|| i->tail_contains(36_n)) );
	}
},

CASE( "Iterating over a nid to get partners" ) {
	Nid n1 = 333_n;
	fabric.get(n1, 34_n).define(78_n);
	fabric.get(n1, 35_n).define(79_n);
	fabric.get(n1, 36_n).define(80_n);

	for (auto i : n1) {
		EXPECT( (i->tail_contains(34_n)
			|| i->tail_contains(35_n)
			|| i->tail_contains(36_n)) );
	}
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

