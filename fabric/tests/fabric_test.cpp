#include "lest.hpp"
#include "dharc/node.hpp"
#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"

using namespace dharc;

/* ==== MOCKS =============================================================== */

Harc::Harc() :
	head_(null_n),
	flags_(static_cast<unsigned char>(Flag::none)),
	dependants_(nullptr) {}


void Harc::addDependant(const Harc &h) {
	dependants_->push_back(&h);
}

const Node &Harc::query() const {
	return head_;
}

void Harc::define(const Node &n) {
	head_ = n;
}

void Harc::define(const vector<Node>& path) {

}

Harc *Harc::instantiate(const Node &any) {
	return this;
}


std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
	os << '<' << n.value << '>';
	return os;
}

/*std::ostream &dharc::operator<<(std::ostream &os, const Harc &h) {
	os << '[' << h.tail().first << ',' << h.tail().second
		<< "->" << h.query() << ']';
	return os;
}*/

/* ==== END MOCKS =========================================================== */

const lest::test specification[] = {

CASE( "Tails are symetric when looking up a harc" ) {
	Fabric::define(Tail{10_n, 11_n}, 55_n);
	EXPECT( Fabric::query(10_n,11_n) == 55_n );
	EXPECT( Fabric::query(11_n,10_n) == 55_n );
	Fabric::define(Tail{11_n, 10_n}, 66_n);
	EXPECT( Fabric::query(10_n,11_n) == 66_n );
	EXPECT( Fabric::query(11_n,10_n) == 66_n );
},

CASE( "Tails greater than 2" ) {
	Fabric::define(Tail{33_n, 34_n, 35_n, 36_n}, 78_n);
	EXPECT( Fabric::query(Tail{33_n, 34_n, 35_n, 36_n}) == 78_n );
	EXPECT( Fabric::query(Tail{33_n, 35_n, 34_n, 36_n}) == 78_n );
},

CASE( "Following a single path, all cases" ) {
	Fabric::define(Tail{1_n, 2_n}, 1000_n);
	Fabric::define(Tail{1000_n, 3_n}, 1001_n);
	Fabric::define(Tail{1001_n, 4_n}, 55_n);
	
	// Normal case
	EXPECT( Fabric::path({Node::reserved(2), Node::reserved(2),
							1_n,2_n,3_n,4_n}) == 55_n );
	// Degenerate case 1
	EXPECT( Fabric::path({}) == null_n );
	// Base case
	EXPECT( Fabric::path({5_n}) == null_n );
},

CASE( "Follow a set of paths in parallel" ) {
	Fabric::define(Tail{220_n, 2_n}, 2000_n);
	Fabric::define(Tail{2000_n, 3_n}, 2001_n);
	Fabric::define(Tail{2001_n, 4_n}, 66_n);
	Fabric::define(Tail{221_n, 2_n}, 2002_n);
	Fabric::define(Tail{2002_n, 66_n}, 77_n);
	
	std::vector<Node> res;
	
	// Normal case, 2 paths
	res = Fabric::paths({
		{Node::reserved(2), 221_n, 2_n, 66_n},
		{Node::reserved(2), Node::reserved(2), 220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 66_n );
	
	// Normal case, 3 paths
	res = Fabric::paths({
		{Node::reserved(2), 221_n, 2_n, 66_n},
		{Node::reserved(2), 221_n, 2_n, 66_n},
		{Node::reserved(2), Node::reserved(2), 220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == 77_n );
	EXPECT( res[2] == 66_n );
	
	// Degen case, 3 paths
	res = Fabric::paths({
		{Node::reserved(2), 221_n, 2_n, 66_n},
		{},
		{Node::reserved(2), Node::reserved(2), 220_n, 2_n, 3_n, 4_n}
	});
	EXPECT( res[0] == 77_n );
	EXPECT( res[1] == null_n );
	EXPECT( res[2] == 66_n );
},


CASE( "Check the change log is filled correctly" ) {
	Fabric::clearChanges();
	Fabric::define(Tail{100_n, 200_n}, 300_n);
	vector<const Tail*> changes;
	Fabric::changes(changes, 10);
	EXPECT( changes.size() == 1U );
	EXPECT( changes[0]->at(0) == 100_n );
},

CASE( "Check that partners are inserted to partner lists") {
	Node n1 = 333_n;

	Fabric::define(Tail{n1, 34_n}, 78_n);
	Fabric::define(Tail{n1, 35_n}, 79_n);
	Fabric::define(Tail{n1, 36_n}, 80_n);

	auto partners = Fabric::partners(n1, 20);
	EXPECT( static_cast<int>(partners.size()) == 3 );
	
	for (auto i : partners) {
		EXPECT( ((i->at(0) == 34_n || i->at(1) == 34_n)
			|| (i->at(0) == 35_n || i->at(1) == 35_n)
			|| (i->at(0) == 36_n || i->at(1) == 36_n)) );
	}
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}

