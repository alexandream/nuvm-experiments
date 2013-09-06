#include <stdint.h>
#include "runner.h"

#include "nuvm.h"
static Suite* create_test_suite (const char* name);

static case_init_t pool[MAX_TEST_CASES];
static uint32_t pool_index = 0;

void push_test_case_constructor(case_init_t init) {
	pool[pool_index] = init;
	pool_index++;
}



int main(int argc, char **argv) {
	// Initialize the library we're testing.
	nuvm_init();

	int number_failed;
	Suite *s = create_test_suite (argv[1]);
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static Suite* create_test_suite (const char* name)
{
	Suite* s = suite_create(name);
	for (int i = 0; i < pool_index; i++) {
		pool[i](s);
	}
	return s;
}
