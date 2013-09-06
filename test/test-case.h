#include <stdlib.h>
#include <check.h>
#include <stdio.h>
#include <stdbool.h>

#include "runner.h"

#ifndef TEST_CASE_NAME
#error TEST_CASE_NAME must be defined before including "test-case.h"
#endif


static TFun  _CT_function_list[MAX_TEST_FUNCTIONS];
static const char* _CT_name_list[MAX_TEST_FUNCTIONS];
static int _CT_last_index = 0;

static void _CT_push_test(TFun test_function, const char* test_name) {
	_CT_function_list[_CT_last_index] = test_function;
	_CT_name_list[_CT_last_index] = test_name;
	_CT_last_index++;
}


static void _CT_init_test_case(Suite * suite) {
	TCase *tcase = tcase_create(TEST_CASE_NAME);

	for (int i = _CT_last_index; i >= 0; i--) {
		TFun tfunc = _CT_function_list[i];
		const char* tname = _CT_name_list[i];
		_tcase_add_test(tcase,tfunc, tname, 0, 0, 0, 1);
	}
	
	suite_add_tcase(suite, tcase);
}


__attribute__((constructor))
static void _CT_constructor() {
	push_test_case_constructor(_CT_init_test_case);
}

#define BEGIN_TEST(__test_name) \
static void __test_name (int _i CK_ATTRIBUTE_UNUSED);\
__attribute__((constructor))\
static void TEST_INITIALIZER__ ## __test_name() {\
	_CT_push_test(__test_name, #__test_name);\
}\
START_TEST(__test_name) 
