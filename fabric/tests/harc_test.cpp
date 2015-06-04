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
fabric::HarcMap Fabric::harcs__;

void Fabric::counterThread() {
	while (true) {
		++counter__;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}



Node dummy_result;

Node Fabric::query(const Tail &tail) {
	return get(tail).query();
}

void Fabric::define(const Tail &tail, const Node &head) {
	get(tail).define(head);
}

void Fabric::define(const Tail &tail, const vector<Node> &path) {
	get(tail).define(path);
}

void Fabric::updatePartners(const Harc *h) {

}


Node Fabric::path(const vector<Node> &p, const Harc *dep) {
	return dummy_result;
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


Harc &Fabric::get(const Tail &key) {
	auto it = harcs__.find(key);

	if (it != harcs__.end()) {
		return *(it->second);
	} else {
		auto h = new Harc();
		h->setIterator(harcs__.insert({key, h}).first);
		return *h;
	}
}

Definition::Definition(const vector<Node> &definition) {
	path_ = definition;
}

Node Definition::evaluate(const Harc *harc) const {
	return Fabric::path(path_, harc);
}

vector<Node> Definition::instantiate(const Node &any) {
	return {};
}


std::ostream &dharc::operator<<(std::ostream &os, const Node &n) {
	os << '<' << n.value << '>';
	return os;
}


std::ostream &dharc::operator<<(std::ostream &os, const Definition &d) {
	return os;
}


Node::operator std::string() const {
	return "";
}


/* ==== END MOCKS ==== */

typedef vector<Node> path_t;

const lest::test specification[] = {

CASE( "Define and then query same value" ) {
	Fabric::define(Tail{123_n, 124_n}, 55_n);
	EXPECT( Fabric::query(123_n, 124_n) == 55_n );
	Fabric::define(Tail{123_n, 124_n}, 77_n);
	EXPECT( Fabric::query(123_n, 124_n) == 77_n );
},

CASE( "A simple one path definition with out-of-date trigger" ) {
	Fabric::define(Tail{102_n, 103_n}, path_t{100_n,101_n});
	dummy_result = 49_n;
	EXPECT( Fabric::query(102_n, 103_n) == 49_n );
	
	dummy_result = 50_n;
	Fabric::define(Tail{100_n, 101_n}, 10_n);
	EXPECT( Fabric::query(102_n, 103_n) == 50_n );
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification, argc, argv);
}
