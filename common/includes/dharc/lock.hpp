/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_LOCK_HPP_
#define DHARC_LOCK_HPP_

#include <thread>
#include <atomic>

namespace dharc {
class Lock {
	public:
	Lock() : lock_(ATOMIC_FLAG_INIT) {}

	void lock() {
		while (lock_.test_and_set()) std::this_thread::yield();
	}

	void unlock() {
		lock_.clear();
	}

	private:
	std::atomic_flag lock_;
};
};  // namespace dharc

#endif  // DHARC_LOCK_HPP_

