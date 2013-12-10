#include <stdio.h>
#include <stdlib.h>

#include "test-suite.h"

ATResultList* at_run_all_tests() {
	int i, j, case_count, suite_count;
	ATCase* tcase;
	ATResult* result;
	ATSuite* suite;
	ATResultList* result_list = at_new_result_list();

	suite_count = at_count_suites();
	for (i = 0; i < suite_count; i++) {
		suite = at_get_nth_suite(i);
		case_count = at_count_cases(suite);
		for (j = 0; j < case_count; j++) {
			tcase = at_get_nth_case(suite, j);
			result = at_execute_case(suite, tcase);
			at_append_result(result_list, result);
		}
	}
	return result_list;
}

void print_results(ATResultList* result_list) {
	int succeeded = 0, failed = 0;
	int result_count = at_count_results(result_list);
	int i, j;
	for(i = 0; i < result_count; i++) {
		ATResult* result = at_get_nth_result(result_list, i);
		int failure_count = at_count_failures(result);
		fprintf(stderr, "[%s]: %s\n",
				(failure_count)?"F":"S",
				at_get_full_name(result));
		for (j = 0; j < failure_count; j++) {
			ATFailure* failure = at_get_nth_failure(result, j);
			fprintf(stderr, "     %s[%d]: %s\n",
					failure->file_name,
					failure->line_number,
					failure->message);
		}
		if (failure_count == 0) {
			succeeded++;
		}
		else {
			failed++;
		}
	}
	fprintf(stderr, "\nTotal: %d succeeded, %d failed.\n",
			succeeded, failed);
}

int main() {
	ATResultList* result_list = at_run_all_tests();
	print_results(result_list);
	return 0;
}

