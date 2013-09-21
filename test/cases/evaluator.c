#define TEST_CASE_NAME "Evaluator"
#include "../test-case.h"

#include "evaluator.h"
#include "module.h"
#include "types/primitive.h"
#include "types/procedure.h"
#include "nuvm.h"

static
nuvm_value_t return_input(void* input, nuvm_value_t dummy) {
	return nuvm_wrap_pointer(input);
}

BEGIN_TEST(test_evaluator_runs_simple_func) {
//	nuvm_primitive_t* primitive_func =
//		nuvm_new_primitive(return_input, (void*) 0xDEADBEEF);
//
//	nuvm_value_t prim = nuvm_wrap_pointer(primitive_func);
	nuvm_value_t dummy;
	dummy = nuvm_wrap_pointer(&dummy);
	
	nuvm_module_builder_t* builder = nuvm_new_module_builder();
	uint32_t ientry = nuvm_module_builder_push_register(builder, dummy);
//	uint32_t iprimitive = nuvm_module_builder_push_register(builder, prim);
	uint32_t icode = 
		nuvm_module_builder_push_instruction(builder,
		                                     nuvm_op_global_ref(0, ientry));

	nuvm_module_builder_push_instruction(builder, nuvm_op_return(0));

	nuvm_module_t* mod = nuvm_module_builder_build(builder, ientry);
	nuvm_destroy_module_builder(builder);
	
	nuvm_procedure_t* proc = nuvm_new_procedure(mod, 0, 1);
	nuvm_module_store_register(mod, ientry, nuvm_wrap_pointer(proc));
	nuvm_evaluator_t* eval = nuvm_new_evaluator();
	
	nuvm_evaluator_setup(eval, mod);

	nuvm_value_t ret = nuvm_evaluator_run(eval);
	nuvm_destroy_module(mod);
	nuvm_destroy_evaluator(eval);

	fail_if(nuvm_unwrap_pointer(ret) != proc);
} END_TEST



BEGIN_TEST(test_evaluator_runs_primitive_func) {
	nuvm_primitive_t* primitive_func =
		nuvm_new_primitive(return_input, (void*) 0xDEADBEEF);

	nuvm_value_t prim = nuvm_wrap_pointer(primitive_func);
	nuvm_value_t dummy;
	dummy = nuvm_wrap_pointer(&dummy);
	
	nuvm_module_builder_t* builder = nuvm_new_module_builder();
	uint16_t iprimitive = nuvm_module_builder_push_register(builder, prim);
	uint16_t ientry = nuvm_module_builder_push_register(builder, dummy);
	uint32_t icode = 
		nuvm_module_builder_push_instruction(builder,
		                                     nuvm_op_global_ref(1, iprimitive));
	nuvm_module_builder_push_instruction(builder, nuvm_op_sva_call(0, 1, 1));
	nuvm_module_builder_push_instruction(builder, nuvm_op_return(0));

	nuvm_module_t* mod = nuvm_module_builder_build(builder, ientry);
	nuvm_destroy_module_builder(builder);
	
	nuvm_procedure_t* proc = nuvm_new_procedure(mod, 0, 2);
	nuvm_module_store_register(mod, ientry, nuvm_wrap_pointer(proc));
	nuvm_evaluator_t* eval = nuvm_new_evaluator();
	nuvm_evaluator_setup(eval, mod);

	nuvm_value_t ret = nuvm_evaluator_run(eval);
	nuvm_destroy_module(mod);
	nuvm_destroy_evaluator(eval);

	fail_if((uint64_t)nuvm_unwrap_pointer(ret) != 0xDEADBEEF);
} END_TEST

