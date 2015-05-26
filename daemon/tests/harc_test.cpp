#include "lest.hpp"

#include "dharc/nid.hpp"
#include "dharc/harc.hpp"
#include "dharc/fabric.hpp"
#include "dharc/definition.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace dharc;
using std::vector;

/* ==== MOCKS ==== */

std::atomic<unsigned long long> Fabric::s_counter(0);

void Fabric::counter_thread() {
	while (true) {
		++s_counter;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

Fabric &dharc::fabric = Fabric::singleton();

Fabric::Fabric() {
}

Fabric::~Fabric() {
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

Nid dummy_result;

Nid Fabric::path(const vector<vector<Nid>> &p, const Harc *dep) {
	if (dep) {
		Harc &h = get(p[0][0],p[0][1]);
		h.add_dependant(*dep);
	}
	return dummy_result;
}

const Harc *last_log = nullptr;

void Fabric::log_change(const Harc *h) {
	last_log = h;
}

Harc &Fabric::get(const pair<Nid, Nid> &key) {
	auto it = m_harcs.find(key);

	if (it != m_harcs.end()) {
		return *(it->second);
	} else {
		auto h = new Harc(key);
		m_harcs.insert({key, h});
		return *h;
	}
}

const Nid &Definition::evaluate(const Harc *harc) const {
	m_cache = fabric.path(m_path, harc);
	return m_cache;
}

Definition *Definition::from_path(const vector<vector<Nid>> &path) {
	Definition *res = new Definition();
	res->m_path = path;
	return res;
}

std::ostream &dharc::operator<<(std::ostream &os, const Nid &n) {
	os << '[' << static_cast<int>(n.t) << ':' << n.i << ']';
	return os;
}

/* ==== END MOCKS ==== */


const lest::test specification[] = {

CASE( "Define and then query same value" ) {
	Harc &h1 = fabric.get(123_n,'g'_n);
	h1.define(55_n);
	EXPECT( h1.query() == 55_n );
	h1.define(77_n);
	EXPECT( h1.query() == 77_n );
},

CASE( "Use of harc assignment operator to define") {
	Harc &h1 = fabric.get(44_n,55_n);
	h1 = 66_n;
	EXPECT( h1.query() == 66_n );
	h1 = 88_n;
	EXPECT( h1.query() == 88_n );
},

CASE( "Comparison of harc to a nid") {
	Harc &h1 = fabric.get(33_n, 22_n);
	h1 = 78_n;
	EXPECT( h1 == 78_n );
	EXPECT( !(h1 == 'a'_n) );
},

CASE( "A simple one path definition with out-of-date trigger" ) {
	Harc &h1 = fabric.get(102_n, 103_n);
	
	h1.define({{100_n,101_n}});
	dummy_result = 49_n;
	EXPECT( h1.query() == 49_n );
	
	dummy_result = 50_n;
	fabric.get(100_n, 101_n).define(10_n);
	EXPECT( h1.query() == 50_n );
},

CASE( "Check and set harc flags" ) {
	Harc &h1 = fabric.get(99_n, 88_n);
	
	EXPECT( h1.check_flag(Harc::Flag::none) );
	
	h1.set_flag(Harc::Flag::log);
	EXPECT( h1.check_flag(Harc::Flag::log) );
	EXPECT( h1.check_flag(Harc::Flag::meta) == false );
	
	h1.set_flag(Harc::Flag::meta);
	EXPECT( h1.check_flag(Harc::Flag::log) );
	EXPECT( h1.check_flag(Harc::Flag::meta) );
	
	h1.clear_flag(Harc::Flag::log);
	EXPECT( h1.check_flag(Harc::Flag::log) == false );
	EXPECT( h1.check_flag(Harc::Flag::meta) );
},

CASE( "Harcs flag log are logged when changed" ) {
	Harc &h = fabric.get(2_n, 3_n);
	
	EXPECT( h.check_flag(Harc::Flag::log) == false );
	last_log = nullptr;
	h.define(7_n);
	EXPECT( last_log == nullptr );
	
	h.set_flag(Harc::Flag::log);
	h.define(8_n);
	EXPECT( last_log == &h );
	
	last_log = nullptr;
	h.define({{109_n,110_n}});
	EXPECT( last_log == &h );
	
	h.clear_flag(Harc::Flag::log);
	last_log = nullptr;
	h.define({{111_n,112_n}});
	EXPECT( last_log == nullptr );
}
};

int main(int argc, char *argv[]) {
	return lest::run(specification);
}
