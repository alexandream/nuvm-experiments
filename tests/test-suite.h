#ifndef __NUVM__TEST__TEST_SUITE_H__
#define __NUVM__TEST__TEST_SUITE_H__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <atest.h>

#ifndef SUITE_NAME
#define SUITE_NAME Untitled
#endif

#define TEST(name) AT_TEST(SUITE_NAME, name)

#define EXPECT(cond)          at_expect(at_result, cond)
#define EXPECT_MSG(cond, msg) at_expect_msg(at_result, cond, msg)
#define ASSERT(cond)          at_assert(at_result, cond)
#define ASSERT_MSG(cond, msg) at_assert_msg(at_result, cond, msg)

#define MF         at_allocf
#endif
