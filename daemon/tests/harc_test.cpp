#include "fdsb/test.hpp"
#include "fdsb/nid.hpp"
#include "fdsb/harc.hpp"
#include "fdsb/fabric.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace fdsb;
using std::vector;

/* ==== MOCKS ==== */

std::atomic<unsigned long long> Fabric::s_counter(0);

void Fabric::counter_thread() {
	while (true) {
		++s_counter;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

Fabric &fdsb::fabric = Fabric::singleton();

Fabric::Fabric() {
}

Fabric::~Fabric() {
}

Fabric &Fabric::singleton() {
	return *(new Fabric());
}

Nid dummy_result;

Nid Fabric::path(const vector<vector<Nid>> &p, Harc *dep) {
	if (dep) {
		Harc &h = get(p[0][0],p[0][1]);
		h.add_dependant(*dep);
	}
	return dummy_result;
}

Harc *last_log = nullptr;

void Fabric::log_change(Harc *h) {
	last_log = h;
}

Harc &Fabric::get(const pair<Nid, Nid> &key) {
	auto it = m_harcs.find(key);

	if (it != m_harcs.end()) {
		return *(it->second);
	} else {
		auto h = new Harc(key);
		m_harcs.insert({key, h});

		// Update node partners to include this harc
		// TODO(knicos): This should be insertion sorted.
		auto &p1 = m_partners[key.first];
		p1.push_front(h);
		h->m_partix[0] = p1.begin();
		auto &p2 = m_partners[key.second];
		p2.push_front(h);
		h->m_partix[1] = p2.begin();

		return *h;
	}
}

/* ==== END MOCKS ==== */

/* Subscript operator overload test:
 * Does the subscript operator for Nid and Harc work as expected.
 */
void test_harc_subscript()
{
	Harc &h1 = 19_n[20_n];
	Harc &h2 = 20_n[19_n];
	
	CHECK(&h1 == &h2);
	DONE;
}

/* Basic Harc creation, definition and query:
 * Make sure Harcs are created correctly and the a constant define and
 * associated query match.
 */
void test_harc_defquery()
{
	Harc &h1 = fabric.get(123_n,'g'_n);
	h1.define(55_n);
	// CHECK(h1.is_out_of_date() == false);
	CHECK(h1.query() == 55_n);
	h1.define(77_n);
	// CHECK(h1.is_out_of_date() == false);
	CHECK(h1.query() == 77_n);
	DONE;
}

/* Check Nid assignment operator:
 * Does assigning a Nid to a Harc perform a constant define.
 */
void test_harc_assign()
{
	Harc &h1 = fabric.get(44_n,55_n);
	h1 = 66_n;
	CHECK(h1.query() == 66_n);
	h1 = 88_n;
	CHECK(h1.query() == 88_n);
	DONE;
}

/* Nid Harc equality operator:
 * If a Harc is compared with a Nid for equality, it should compare the head
 * with the Nid.
 */
void test_harc_eqnid()
{
	Harc h1 = fabric.get(33_n, 22_n);
	h1 = 78_n;
	CHECK(h1 == 78_n);
	CHECK(!(h1 == 'a'_n));
	DONE;
}

/* Check a basic definition
 */
void test_harc_definition()
{
	Harc &h1 = 102_n[103_n];
	
	h1.define({{100_n,101_n}});
	// CHECK(h1.is_out_of_date());
	dummy_result = 49_n;
	CHECK(h1.query() == 49_n);
	// CHECK(102_n[103_n].is_out_of_date() == false);
	
	dummy_result = 50_n;
	CHECK(h1.query() == 49_n);
	
	100_n[101_n].define(10_n);
	// CHECK(h1.is_out_of_date());
	CHECK(h1.query() == 50_n);
	DONE;
}

/*
 * Make sure flags set, check and clear correctly.
 */
void test_harc_flags() {
	Harc &h1 = 99_n[88_n];
	
	CHECK(h1.check_flag(Harc::Flag::none));
	
	h1.set_flag(Harc::Flag::log);
	CHECK(h1.check_flag(Harc::Flag::log));
	CHECK(h1.check_flag(Harc::Flag::meta) == false);
	
	h1.set_flag(Harc::Flag::meta);
	CHECK(h1.check_flag(Harc::Flag::log));
	CHECK(h1.check_flag(Harc::Flag::meta));
	
	h1.clear_flag(Harc::Flag::log);
	CHECK(h1.check_flag(Harc::Flag::log) == false);
	CHECK(h1.check_flag(Harc::Flag::meta));
	DONE;
}

void test_harc_log() {
	Harc &h = 2_n[3_n];
	
	CHECK(h.check_flag(Harc::Flag::log) == false);
	last_log = nullptr;
	h.define(7_n);
	CHECK(last_log == nullptr);
	
	h.set_flag(Harc::Flag::log);
	h.define(8_n);
	CHECK(last_log == &h);
	
	last_log = nullptr;
	h.define({{109_n,110_n}});
	CHECK(last_log == &h);
	
	h.clear_flag(Harc::Flag::log);
	last_log = nullptr;
	h.define({{111_n,112_n}});
	CHECK(last_log == nullptr);
	DONE;
}

int main(int argc, char *argv[]) {
	test(test_harc_defquery);
	test(test_harc_assign);
	test(test_harc_eqnid);
	test(test_harc_subscript);
	test(test_harc_definition);
	test(test_harc_flags);
	test(test_harc_log);
	return test_fail_count();
}
