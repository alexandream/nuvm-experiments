#ifndef NHG_T_TEST_SUITE
#define NHG_T_TEST_SUITE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <atest.h>

#include "common/errors.h"

typedef struct {
	bool success;
	const char* message;
} NCheckResult;

#ifndef SUITE_NAME
#define SUITE_NAME Untitled
#endif

#define CONSTRUCTOR AT_CONSTRUCTOR(SUITE_NAME)
#define SETUP       AT_SETUP(SUITE_NAME, ni_g_test_result)
#define TEARDOWN    AT_TEARDOWN(SUITE_NAME, ni_g_test_result)
#define TEST(name)  AT_TEST(SUITE_NAME, name, ni_g_test_result)

#define SIGNAL_CONSTRUCTOR_ERROR() return

#define MF         at_allocf

#define CHECK(check) do { \
	NCheckResult result = check; \
	at_check_msg(ni_g_test_result, result.success, result.message); \
} while(0)

#define ASSERT(check) do { \
	NCheckResult result = check; \
	at_check_msg(ni_g_test_result, result.success, result.message); \
	if (result.success == false) { \
		return; \
	} \
} while (0)

#define NI_DO_CHECK_F(CC, MM, FF) do { \
	NCheckResult result; \
	result.success = CC; \
	result.message = result.success ? NULL : MM; \
	FF\
	return result; \
} while(0)

#define NI_DO_CHECK(CC, MM) NI_DO_CHECK_F(CC, MM, {})

NCheckResult
IS_TRUE_IMPL(const char* expression, bool condition);

NCheckResult
IS_FALSE_IMPL(const char* expression, bool condition);

NCheckResult
EQ_INT_IMPL(const char* expression, int64_t value, int64_t expected);

NCheckResult
EQ_DBL_IMPL(const char* expression, double value, double expected);

NCheckResult
EQ_STR_IMPL(const char* expression, const char* value, const char* expected);

NCheckResult
EQ_SZT_IMPL(const char* expression, size_t value, size_t expected);

NCheckResult
EQ_UINT_IMPL(const char* expression, uint64_t value, uint64_t expected);

NCheckResult
ERROR_OK(NError* error);

NCheckResult
HAS_ERROR(NError* error, const char* expected_name);

#ifdef USE_JOINED_STRING

static const char*
JOINED_STRING(char* dst, const char **src, size_t src_size) {
	size_t n_elems = src_size / sizeof(const char*);
	size_t i, offset = 0;
	for (i = 0; i < n_elems; i++) {
		strcpy(dst + offset, src[i]);
		offset += strlen(src[i]);
	}
	return dst;
}

#define EXPECTED_SIZE(VAR, LINE_LENGTH)\
	LINE_LENGTH * sizeof(VAR)/sizeof(char*)

#endif

#define IS_TRUE(VV)      IS_TRUE_IMPL(#VV, VV)
#define IS_FALSE(VV)     IS_FALSE_IMPL(#VV, VV)
#define EQ_INT(VV, EE)   EQ_INT_IMPL(#VV, VV, EE)
#define EQ_DBL(VV, EE)   EQ_DBL_IMPL(#VV, VV, EE)
#define EQ_STR(VV, EE)   EQ_STR_IMPL(#VV, VV, EE)
#define EQ_SZT(VV, EE)   EQ_SZT_IMPL(#VV, VV, EE)
#define EQ_UINT(VV, EE)  EQ_UINT_IMPL(#VV, VV, EE)
#endif
