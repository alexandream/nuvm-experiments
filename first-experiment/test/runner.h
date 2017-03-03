#include <stdlib.h>
#include <check.h>

#ifndef MAX_TEST_CASES
	#define MAX_TEST_CASES 256
#endif

#ifndef MAX_TEST_FUNCTIONS
	#define MAX_TEST_FUNCTIONS 256
#endif

typedef void (*case_init_t)(Suite*);

void push_test_case_constructor(case_init_t init);
