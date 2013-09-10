#define TEST_CASE_NAME "Module Type"
#include <stdio.h>

#include "../test-case.h"

#include "nuvm.h"
#include "module.h"


BEGIN_TEST(test_module_builder) {
	nuvm_module_builder_t* builder = nuvm_new_module_builder();

	nuvm_value_t vnull = nuvm_wrap_pointer(NULL);
	nuvm_value_t vbuilder = nuvm_wrap_pointer(builder);

	nuvm_module_builder_push_register(builder, vnull);
	uint16_t ibuilder = nuvm_module_builder_push_register(builder, vbuilder);

	nuvm_module_t* mod = nuvm_module_builder_build(builder, ibuilder);
	
	fail_if(mod == NULL);

	nuvm_destroy_module_builder(builder);

	nuvm_value_t reg0 = nuvm_module_load_register(mod, 0);
	nuvm_value_t reg1 = nuvm_module_load_register(mod, 1);
	nuvm_value_t entry_point = nuvm_module_load_entry_point(mod);
	fail_unless(nuvm_is_equal(reg0, vnull));
	fail_unless(nuvm_is_equal(reg1, vbuilder));
	fail_unless(nuvm_is_equal(entry_point, reg1));
} END_TEST



