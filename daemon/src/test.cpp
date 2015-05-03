#include "fdsb/test.hpp"
#include <stdio.h>

unsigned int check_count = 0;
unsigned int check_failed = 0;
unsigned int test_count = 0;
unsigned int test_failed = 0;
int test_pass = 1;
int test_pretty = 1;

void fdsb_test_done(const char *function)
{
	if (test_pass == 1)
	{
		printf("%s passed\n",function);
	}
	else
	{
		printf("%s failed\n",function);
	}
}

void fdsb_test_checkfailed(int line, const char *function, const char *file)
{
	printf("%s:%d failed!\n",function,line);

	test_pass = 0;
	check_count++;
	check_failed++;
}

void fdsb_test_checkpassed()
{
	check_count++;
}

void fdsb_test(void (*test)(void))
{
	test_pass = 1;
	test_count++;
	test();
}

