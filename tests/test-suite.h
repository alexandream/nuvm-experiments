#ifndef __NUVM__TEST__TEST_SUITE_H__
#define __NUVM__TEST__TEST_SUITE_H__
#include <stdio.h>
#include <stdlib.h>
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

#define ASSERT_CHECK(check) do { \
	NCheckResult result = check; \
	at_check_msg(ni_g_test_result, result.success, result.message); \
	if (result.success == false) { \
		return; \
	} \
} while (0)

#define NI_MAKE_CHECK(CC, MM, FF) do { \
	NCheckResult result; \
	result.success = CC; \
	result.message = result.success ? NULL : MM; \
	return result; \
} while(0)


NCheckResult
CK_BOOL(const char* expression, bool condition);

NCheckResult
CK_BOOL_MSG(const char* expression, bool condition, const char* msg);


NCheckResult
CK_EQ_I64(const char* expression, int64_t value, int64_t expected);

NCheckResult
CK_ERR_OK(const char* expression, NError* error);

#define ASSERT(VV) ASSERT_CHECK(CK_BOOL(#VV, VV))
#define ASSERT_EQ_I64(VV, EE) ASSERT_CHECK(CK_EQ_I64(#VV, VV, EE))
#define ASSERT_EOK(VV) ASSERT_CHECK(CK_ERR_OK(#VV, VV))


#define ASSERT_MSG(condition, msg) do { \
	bool _condition = condition; \
	const char* _msg = NULL;\
	if (_condition) _msg = msg; \
	if (at_check_msg(ni_g_test_result, _condition, msg)) return; \
} while(0)


#endif
