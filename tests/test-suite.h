#ifndef __NUVM__TEST__TEST_SUITE_H__
#define __NUVM__TEST__TEST_SUITE_H__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <atest.h>

#include "common/errors.h"

#ifndef SUITE_NAME
#define SUITE_NAME Untitled
#endif

#define CONSTRUCTOR AT_CONSTRUCTOR(SUITE_NAME)
#define SETUP       AT_SETUP(SUITE_NAME)
#define TEARDOWN    AT_TEARDOWN(SUITE_NAME)
#define TEST(name)  AT_TEST(SUITE_NAME, name)

#define SIGNAL_CONSTRUCTOR_ERROR() return

#define EXPECT(cond)          at_expect(at_result, cond)
#define EXPECT_MSG(cond, msg) at_expect_msg(at_result, cond, msg)
#define ASSERT(cond)          at_assert(at_result, cond)
#define ASSERT_MSG(cond, msg) at_assert_msg(at_result, cond, msg)
#define ASSERT_MSG_O(cond, msg) at_assert_msg(at_result, cond, msg);

#define MF         at_allocf

#define ASSERT_EQ_I64(value, expected) do {                          \
	int64_t _value = (int64_t) value;                              \
	int64_t _expected = (int64_t) expected;                        \
	ASSERT_MSG_O(_value == _expected,                              \
		MF("Expression (%s) expected to have value %ld but found " \
		   "to have value %ld.",                                   \
		   #value, _expected, _value))                             \
} while(0)

#define ASSERT_EOK(error_ptr) do {                                  \
	NError* _error = error_ptr;                                     \
	ASSERT_MSG_O(n_error_ok(_error),                                \
		MF("Error (%s) expected to be ok, but the following error " \
		   "was reported: \"%s\"",                                  \
		   #error_ptr, n_error_print(_error)))                      \
} while(0)
#endif
