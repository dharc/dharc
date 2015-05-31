#include "lest.hpp"
#include "dharc/node.hpp"
#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;

/* ==== MOCKS =============================================================== */

Harc::Harc(const pair<Node, Node> &t) :
	tail_(t),
	flags_(static_cast<unsigned char>(Flag::none)),
	head_(null_n),
	lastquery_(Fabric::counter()),
	strength_(0.0),
	dependants_(nullptr) {}


void Harc::addDependant(const Harc &h) {
	dependants_->push_back(&h);
}

float Harc::significance() const { return 0.0f; }

const Node &Harc::query() {
	return head_;
}

const Node &Harc::query() const {
	return head_;
}

void Harc::define(const Node &n) {
	if (checkFlag(Flag::log)) fabric.logChange(this);

	head_ = n;
}

Harc *Harc::instantiate(const Node &any) {
	return this;
}


std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
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
	fabric.get(10_n,11_n).define(55_n);
	EXPECT( fabric.get(10_n,11_n).query() == 55_n );
	EXPECT( fabric.get(11_n,10_n).query() == 55_n );
	fabric.get(11_n,10_n).define(66_n);
	EXPECT( fabric.get(10_n,11_n).query() == 66_n );
	EXPECT( fabric.get(11_n,10_n).query() == 66_n );
	
	// Check with different Node types
	fabric.get('a'_n, 33_n).define('b'_n);
	EXPECT( fabric.get('a'_n, 33_n).query() == 'b'_n );
	EXPECT( fabric.get(33_n, 'a'_n).query() == 'b'_n );
},

CASE( "Automatically create an non-existing harc" ) {
	fabric.get(15_n,16_n).define(67_n);
	EXPECT( fabric.get(15_n,16_n).query() == 67_n );
},

CASE( "Following a single path, all cases" ) {
	fabric.get(1_n, 2_n).define(1000_n);
	fabric.get(1000_n, 3_n).define(1001_n);
	fabric.get(1001_n, 4_n).define(55_n);
	
	// Normal case
	EXPECT( fabric.path({1_n,2_n,3_n,4_n}) == 55_n );
	// Degenerate case 1
	EXPECT( fabric.path({}) == null_n );
	// Base case
	EXPECT( fabric.path({5_n}) == 5_n );
},

CASE( "Follow a set of paths in parallel" ) {
	fabric.get(220_n, 2_n).define(2000_n);
	fabric.get(2000_n, 3_n).define(2001_n);
	fabric.get(2001_n, 4_n).define(66_n);
	fabric.get(221_n, 2_n).define(2002_n);
	fabric.get(2002_n, 66_n).define(77_n);
	
	std::vector<Node> res;
	
	// Normal case, 2 paths
	res = fabric.paths({
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 66_n );
	
	// Normal case, 3 paths
	res = fabric.paths({
		{221_n, 2_n, 66_n},
		{221_n, 2_n, 66_n},
		{220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 77_n );
	EXPECT( res[2] == 66_n );
	
	// Degen case, 3 paths
	res = fabric.paths({
		{221_n, 2_n, 66_n},
		{},
		{220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == null_n );
	EXPECT( res[2] == 66_n );
},


CASE( "Check the change log is filled correctly" ) {
	auto chg = fabric.changes();
	fabric.get(40_n,41_n).startRecording();
	fabric.get(40_n,42_n).startRecording();
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
	Node n1 = 333_n;

	fabric.get(n1, 34_n).define(78_n);
	fabric.get(n1, 35_n).define(79_n);
	fabric.get(n1, 36_n).define(80_n);

	auto partners = fabric.partners(n1);
	EXPECT( static_cast<int>(partners.size()) == 3 );
	
	for (auto i : partners) {
		EXPECT( (i->tailContains(34_n)
			|| i->tailContains(35_n)
			|| i->tailContains(36_n)) );
	}
},

CASE( "Iterating over a nid to get partners" ) {
	Node n1 = 333_n;
	fabric.get(n1, 34_n).define(78_n);
	fabric.get(n1, 35_n).define(79_n);
	fabric.get(n1, 36_n).define(80_n);

	for (auto i : n1) {
		EXPECT( (i->tailContains(34_n)
			|| i->tailContains(35_n)
			|| i->tailContains(36_n)) );
	}
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}

