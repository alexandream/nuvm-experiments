#include "test-suite.h"

NCheckResult
IS_TRUE_IMPL(const char* expression, bool condition) {
	NI_MAKE_CHECK(condition,
		MF("Expression (%s) expected to be true but found false.",
		   expression));
}


NCheckResult
EQ_I64_IMPL(const char* expression, int64_t value, int64_t expected) {
	NI_MAKE_CHECK(value == expected,
		MF("Expression (%s) expected to have value %ld but found "
		   "to have value %ld.",
		   expression, expected, value));
}


NCheckResult
ERROR_OK(NError* error) {
	const char* error_text = n_error_print(error);
	NI_MAKE_CHECK_F(n_error_ok(error),
		MF("Error expected to be Ok, but the following error "
		   "was reported: \"%s\"", error_text),
		{
			free(error_text);
		});
}


NCheckResult
EQ_DBL_IMPL(const char* expression, double value, double expected) {
	NI_MAKE_CHECK(value == expected,
		MF("Expression (%s) expected to have value %lf but found "
		   "to have value %lf.",
		   expression, expected, value));
}


NCheckResult
EQ_STR_IMPL(const char* expression, const char* value, const char* expected) {
	NI_MAKE_CHECK(value == NULL && expected == NULL ||
	             (value != NULL && expected != NULL &&
	              strcmp(value, expected) == 0),
		MF("Expression (%s) expected to have value [%s] but found "
		   "to have value [%s].",
		   expression, expected, value));
}


NCheckResult
HAS_ERROR(NError* error, const char* expected_name) {
	const char* actual_name = n_error_print_type_name(error);
	uint32_t expected_id = n_find_error_type(expected_name);
	NI_MAKE_CHECK_F(error->type == expected_id,
		MF("Error expected to be of type %s, but found to be "
		   "of type %s.",
		   expected_name, actual_name),
		{
			free(actual_name);
		});
}
