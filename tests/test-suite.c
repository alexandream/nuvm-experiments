#include "test-suite.h"

NCheckResult
CK_BOOL(const char* expression, bool condition) {
	NI_MAKE_CHECK(condition,
		MF("Expression (%s) expected to be true but found false.",
		   expression), { /* empty finalizer */ });
}


NCheckResult
CK_EQ_I64(const char* expression, int64_t value, int64_t expected) {
	NI_MAKE_CHECK(value == expected,
		MF("Expression (%s) expected to have value %ld but found "
		   "to have value %ld.",
		   expression, expected, value), { /* empty finalizer */ });
}

NCheckResult
CK_ERR_OK(const char* expression, NError* error) {
	const char* error_text = n_error_print(error);
	NI_MAKE_CHECK(n_error_ok(error),
		MF("Error (%s) expected to be ok, but the following error "
		   "was reported: \"%s\"",
		   expression, error_text),
		{ 
			free(error_text); 
		});
}
