#include <stdio.h>
#include <string.h>
#include "common/std-types.h"

#include "test-suite.h"

ATResultList* at_run_all_tests(char* test_name) {
	char buffer[1024];
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
			bool run_test = true;
			tcase = at_get_nth_case(suite, j);
			if (test_name != NULL) {
				sprintf(buffer, "%s.%s", suite->name, tcase->name);
				run_test = strcmp(buffer, test_name) == 0;
			}
			if (run_test) {
				result = at_execute_case(suite, tcase);
				at_append_result(result_list, result);
			}
		}
	}
	return result_list;
}

void _print_results(ATResultList* result_list, bool show_failure_msgs,
                                               bool show_success) {
	int result_count = at_count_results(result_list);
	int i, j;
	int printed_count = 0;
	for(i = 0; i < result_count; i++) {
		ATResult* result = at_get_nth_result(result_list, i);
		int failure_count = at_count_failures(result);
		if (failure_count > 0 || show_success) {
			printed_count++;
			fprintf(stderr, "[%s]: %s\n",
					(failure_count)?"F":"S",
					at_get_full_name(result));
			if (show_failure_msgs) {
				for (j = 0; j < failure_count; j++) {
					ATFailure* failure = at_get_nth_failure(result, j);
					fprintf(stderr, "     %s[%d]: %s\n\n",
							failure->file_name,
							failure->line_number,
							failure->message);
				}
			}
		}
	}
	if (printed_count > 0) {
		puts("");
	}
}

void _count_results(ATResultList* list, int* success, int* failure) {
	int result_count = at_count_results(list);
	int i;
	(*success) = 0;
	(*failure) = 0;
	for (i = 0; i < result_count; i++) {
		ATResult* result = at_get_nth_result(list, i);
		int failure_count = at_count_failures(result);
		if (failure_count > 0) {
			(*failure)++;
		}
		else {
			(*success)++;
		}
	}
}

void print_compact_results(ATResultList* result_list) {
	_print_results(result_list, false, false);
}

void print_results(ATResultList* result_list) {
	_print_results(result_list, true, false);
}

void print_result_totals(ATResultList* result_list) {
	int succeeded, failed;
	_count_results(result_list, &succeeded, &failed);
	fprintf(stderr, "Total: %d; Succeeded: %d; Failed: %d.\n",
			(succeeded+failed), succeeded, failed);
}

int main(int argc, char** argv) {
	char* test_name = (argc > 1) ? argv[1] : NULL;
	ATResultList* result_list;

	result_list = at_run_all_tests(test_name);

	print_results(result_list);
	print_result_totals(result_list);

	return 0;
}
