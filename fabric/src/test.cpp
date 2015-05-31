/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/test.hpp"

#include <string.h>
#include <iostream>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define NOCOLOR "\033[0m"

unsigned int check_count = 0;
unsigned int check_failed = 0;
unsigned int test_count = 0;
unsigned int test_failed = 0;
int test_pass = 1;
int test_pretty = 1;

int dharc::test_fail_count() {
	return check_failed;
}

void dharc::test_done(const char *function) {
	if (strncmp(function, "test_", 5) == 0) {
		function = function+5;
	}
	int dots = 30 - strlen(function);


	if (test_pass == 1) {
		std::cout << function << " ";
		for (int i = 0; i < dots; ++i) std::cout << '.';
		std::cout << " [" << GREEN << "passed" << NOCOLOR << "]" << std::endl;
	} else {
		std::cout << function << " ";
		for (int i = 0; i < dots; ++i) std::cout << '.';
		std::cout << " [" << RED << "failed" << NOCOLOR << "]" << std::endl;
	}
}

void dharc::test_checkfailed(int line, const char *function, const char *file) {
	std::cout << function << ":" << line << " failed!" << std::endl;

	test_pass = 0;
	check_count++;
	check_failed++;
}

void dharc::test_checkpassed() {
	check_count++;
}

void dharc::test(void (*t)(void)) {
	test_pass = 1;
	test_count++;
	t();
}

