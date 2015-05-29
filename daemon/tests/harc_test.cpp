#include "lest.hpp"

#include "dharc/node.hpp"
#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"
#include "dharc/definition.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <sstream>

using namespace dharc;
using std::vector;

/* ==== MOCKS ==== */

std::atomic<unsigned long long> Fabric::counter__(0);

void Fabric::counterThread() {
	while (true) {
		++counter__;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

Fabric dharc::fabric;

Fabric::Fabric() {
}

Fabric::~Fabric() {
}

void Fabric::updatePartners(const Node &n, list<Harc*>::iterator &it) {
}

Node dummy_result;

Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	return p[0];
}

vector<Node> Fabric::paths(const vector<vector<Node>> &p, const Harc *dep) {
	vector<Node> results;

	Harc &h = get(p[0][0],p[0][1]);
	if (dep) {
		h.addDependant(*dep);
	}
	results.push_back(dummy_result);
	return results;
}

const Harc *last_log = nullptr;

void Fabric::logChange(const Harc *h) {
	last_log = h;
}

Harc &Fabric::get(const pair<Node, Node> &key) {
	auto it = harcs_.find(key);

	if (it != harcs_.end()) {
		return *(it->second);
	} else {
		auto h = new Harc(key);
		harcs_.insert({key, h});
		return *h;
	}
}

Definition::Definition(const vector<vector<Node>> &definition) {
	path_ = definition;
}

Node Definition::evaluate(const Harc *harc) const {
	vector<Node> aggregate = fabric.paths(path_, harc);
	return fabric.path(aggregate, harc);
}

vector<vector<Node>> Definition::instantiate(const Node &any) {
	return {{}};
}


std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
	os << '[' << static_cast<int>(n.t) << ':' << n.i << ']';
	return os;
}


std::ostream &dharc::operator<<(std::ostream &os, const Definition &d) {
	return os;
}

Node::operator string() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

Node::operator int() const {
	switch(t) {
	case Type::integer : return static_cast<int>(i);
	default            : return 0;
	}
}

/* ==== END MOCKS ==== */

typedef vector<vector<Node>> path_t;

const lest::test specification[] = {

CASE( "Define and then query same value" ) {
	Harc &h1 = fabric.get(123_n,'g'_n);
	h1.define(55_n);
	EXPECT( h1.query() == 55_n );
	h1.define(77_n);
	EXPECT( h1.query() == 77_n );
},

CASE( "A simple one path definition with out-of-date trigger" ) {
	Harc &h1 = fabric.get(102_n, 103_n);
	
	h1.define(path_t{{100_n,101_n}});
	dummy_result = 49_n;
	EXPECT( h1.query() == 49_n );
	
	dummy_result = 50_n;
	fabric.get(100_n, 101_n).define(10_n);
	EXPECT( h1.query() == 50_n );
},

CASE( "Harcs flag log are logged when changed" ) {
	Harc &h = fabric.get(2_n, 3_n);
	
	EXPECT( h.checkFlag(Harc::Flag::log) == false );
	last_log = nullptr;
	h.define(7_n);
	EXPECT( last_log == nullptr );
	
	h.startRecording();
	h.define(8_n);
	EXPECT( last_log == &h );
	
	last_log = nullptr;
	h.define(path_t{{109_n,110_n}});
	EXPECT( last_log == &h );
	
	h.stopRecording();
	last_log = nullptr;
	h.define(path_t{{111_n,112_n}});
	EXPECT( last_log == nullptr );
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}
