#include <stdio.h>
#include <stdlib.h>

#include "test-suite.h"

int main() {
	int suite_count;
	int i;

	suite_count = at_count_suites();
	printf("Found %d suites.\n", suite_count);
	for (i = 0; i < suite_count; i++) {
		ATSuite* suite = at_get_nth_suite(i);
		int case_count = at_count_cases(suite);
		int j;
		printf(" Suite \"%s\" has %d test cases:\n", suite->name, case_count);
		for (j = 0; j < case_count; j++) {
			ATCase* tcase = at_get_nth_case(suite, j);
			printf("  %s.%s;\n", suite->name, tcase->name);
		}
	}
	return 0;
}

