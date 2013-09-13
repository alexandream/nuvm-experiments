#define TEST_CASE_NAME "Evaluator"
#include "../test-case.h"

#include "evaluator.h"
#include "module.h"
#include "types/primitive.h"
#include "nuvm.h"

static
nuvm_value_t return_input(void* input) {
	return nuvm_wrap_pointer(input);
}

BEGIN_TEST(test_evaluator_runs_simple_func) {
	nuvm_primitive_t *load_func =
		nuvm_new_primitive(return_input, (void*) 0xDEADBEEF);

	nuvm_value_t load_val = nuvm_wrap_pointer(load_func);
	
	nuvm_module_builder_t* builder = nuvm_new_module_builder();
	uint32_t entry = nuvm_module_builder_push_register(builder, load_val);

	nuvm_module_t* mod = nuvm_module_builder_build(builder, entry);
	nuvm_destroy_module_builder(builder);

	nuvm_evaluator_t* eval = nuvm_new_evaluator();
	
	nuvm_value_t ret = nuvm_evaluator_run_module(eval, mod);
	nuvm_destroy_module(mod);
	nuvm_destroy_evaluator(eval);


	fail_if((uint64_t) nuvm_unwrap_pointer(ret) != 0xDEADBEEF);
} END_TEST



