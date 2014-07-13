#ifndef __NUVM__TEST__TEST_SUITE_H__
#define __NUVM__TEST__TEST_SUITE_H__
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

#define NI_MAKE_CHECK_F(CC, MM, FF) do { \
	NCheckResult result; \
	result.success = CC; \
	result.message = result.success ? NULL : MM; \
	return result; \
} while(0)

#define NI_MAKE_CHECK(CC, MM) NI_MAKE_CHECK_F(CC, MM, {})

NCheckResult
IS_TRUE_IMPL(const char* expression, bool condition);

NCheckResult
EQ_I64_IMPL(const char* expression, int64_t value, int64_t expected);

NCheckResult
EQ_DBL_IMPL(const char* expression, double value, double expected);

NCheckResult
EQ_STR_IMPL(const char* expression, const char* value, const char* expected);

NCheckResult
ERROR_OK(NError* error);

NCheckResult
HAS_ERROR(NError* error, const char* expected_name);

#define IS_TRUE(VV)      IS_TRUE_IMPL(#VV, VV)
#define EQ_I64(VV, EE)    EQ_I64_IMPL(#VV, VV, EE)
#define EQ_DBL(VV, EE)    EQ_DBL_IMPL(#VV, VV, EE)
#define EQ_STR(VV, EE)    EQ_STR_IMPL(#VV, VV, EE)

#endif
