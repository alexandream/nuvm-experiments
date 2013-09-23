#define TEST_CASE_NAME "Evaluator"
#include "../test-case.h"

#include "evaluator.h"
#include "module.i.h"
#include "library.h"
#include "types/primitive.h"
#include "types/procedure.h"
#include "nuvm.h"

static
nuvm_value_t return_input(void* input, nuvm_value_t dummy) {
	return nuvm_wrap_pointer(input);
}

BEGIN_TEST(test_evaluator_runs_simple_func) {
	nuvm_value_t dummy;
	dummy = nuvm_wrap_pointer(&dummy);

	nuvm_module_builder_t* builder = nuvm_new_module_builder();
	uint32_t ientry = nuvm_module_builder_push_register(builder, dummy);
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

nuvm_value_t _simple_adder(nuvm_evaluator_t* eval,
                                   uint16_t a,
                                   uint16_t b) {
	static nuvm_primitive_t* pinc = NULL;
	static nuvm_primitive_t* pzerop = NULL;
	static nuvm_primitive_t* pdec = NULL;
	if (pinc == NULL)
		pinc = nuvm_new_primitive(nuvm_primitive_successor, NULL);
	if (pzerop == NULL)
		pzerop = nuvm_new_primitive(nuvm_primitive_is_zero, NULL);
	if (pdec == NULL)
		pdec = nuvm_new_primitive(nuvm_primitive_predecessor, NULL);
	nuvm_value_t inc = nuvm_wrap_pointer(pinc);
	nuvm_value_t zerop = nuvm_wrap_pointer(pzerop);
	nuvm_value_t dec = nuvm_wrap_pointer(pdec);

	nuvm_module_t* mod = nuvm_new_blank_module(6, 20);

	register_pool_t* regpool = &mod->registers;
	code_segment_t*  codeseg = &mod->code_segment;

	register_pool_t_set(regpool, 0, inc);
	register_pool_t_set(regpool, 1, dec);
	register_pool_t_set(regpool, 2, zerop);
	register_pool_t_set(regpool, 3, nuvm_wrap_fixnum((int32_t) a));
	register_pool_t_set(regpool, 4, nuvm_wrap_fixnum((int32_t) b));

#define CODE(INDEX, VALUE) code_segment_t_set(codeseg, INDEX, VALUE)
	CODE( 0, nuvm_op_global_ref(0, 0)); // inc
	CODE( 1, nuvm_op_global_ref(1, 1)); // dec
	CODE( 2, nuvm_op_global_ref(2, 2)); // zerop
	CODE( 3, nuvm_op_global_ref(3, 3)); // a
	CODE( 4, nuvm_op_global_ref(4, 4)); // b
	CODE( 5, nuvm_op_sva_call(5, 2, 3)); // sva-call acc zerop a
	CODE( 6, nuvm_op_jump_if(5, +4));
	CODE( 7, nuvm_op_sva_call(3, 1, 3));
	CODE( 8, nuvm_op_sva_call(4, 0, 4));
	CODE( 9, nuvm_op_jump(-4));
	CODE(10, nuvm_op_return(4));
#undef CODE

	nuvm_value_t proc = nuvm_wrap_pointer(nuvm_new_procedure(mod, 0, 6));
	register_pool_t_set(regpool, 5, proc);
	mod->entry_point = 5;
	nuvm_evaluator_setup(eval, mod);
	nuvm_value_t result = nuvm_evaluator_run(eval);
	nuvm_destroy_module(mod);
	return result;
}

BEGIN_TEST(test_evaluator_runs_simple_adder) {
	nuvm_evaluator_t* eval = nuvm_new_evaluator();

	nuvm_value_t result;
	result = _simple_adder(eval, 2, 2);

	fail_unless(nuvm_is_fixnum(result));
	fail_unless(nuvm_unwrap_fixnum(result) == 4);
} END_TEST
