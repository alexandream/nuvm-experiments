#define AT_SUITE_NAME Sample_Programs
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "value.h"

#include "evaluator.h"
#include "module.h"
#include "memory.h"

#include "objects/primitives.h"
#include "objects/procedures.h"

static NValue
increment_fixnum(void*data, NValue arg, NError*);

static NValue
run_value_selection_if(NValue condition, NValue v1, NValue v2);

static NValue
run_value_selection_unless(NValue condition, NValue v1, NValue v2);

static NValue
test_constant_runner(NValue constant);

static void
test_increment_fixnum_using_primitive(int32_t fixnum);

static NValue
toggle_bool(void*, NValue arg, NError*);

static NValue
wrap_input(void*, NValue arg, NError*);



TEST(choose_between_two_global_values_using_if) {
	NValue f12345 = n_wrap_fixnum(12345);
	NValue f54321 = n_wrap_fixnum(54321);
	NValue result;

	result = run_value_selection_if(N_TRUE, f12345, f54321);
	EXPECT(n_is_equal(result, f12345));

	result = run_value_selection_if(f54321, f12345, f54321);
	EXPECT(n_is_equal(result, f12345));

	result = run_value_selection_if(N_FALSE, f12345, f54321);
	EXPECT(n_is_equal(result, f54321));
}


TEST(choose_between_two_global_values_using_unless) {
	NValue f12345 = n_wrap_fixnum(12345);
	NValue f54321 = n_wrap_fixnum(54321);
	NValue result;

	result = run_value_selection_unless(N_TRUE, f12345, f54321);
	EXPECT(n_is_equal(result, f54321));

	result = run_value_selection_unless(f54321, f12345, f54321);
	EXPECT(n_is_equal(result, f54321));

	result = run_value_selection_unless(N_FALSE, f12345, f54321);
	EXPECT(n_is_equal(result, f12345));
}


TEST(constant_evaluator_returns_constant) {
	NValue* ptr1 = n_alloc_unmanaged(sizeof(NValue));
	NValue* ptr2 = n_alloc_unmanaged(sizeof(NValue));
	NValue val1 = n_wrap_pointer(ptr1);
	NValue val2 = n_wrap_pointer(ptr2);

	EXPECT(n_is_equal(val1, test_constant_runner(val1)));
	EXPECT(n_is_equal(val2, test_constant_runner(val2)));

	n_free(ptr1);
	n_free(ptr2);
}


TEST(increment_fixnum_using_primitive) {
	test_increment_fixnum_using_primitive(0);
	test_increment_fixnum_using_primitive(1);
	test_increment_fixnum_using_primitive(1234567899);
	test_increment_fixnum_using_primitive(-1234567899);
}


TEST(primitive_evaluator_runs_primitive) {
	NValue result;

	NObject* ptr1 = n_alloc_unmanaged(sizeof(NObject));
	NPrimitive* prim1 = n_primitive_new(wrap_input, ptr1, NULL);

	NObject* ptr2 = n_alloc_unmanaged(sizeof(NObject));
	NPrimitive* prim2 = n_primitive_new(wrap_input, ptr2, NULL);

	bool flag = false;
	NPrimitive* prim3 = n_primitive_new(toggle_bool, &flag, NULL);

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(1, 0, 0, NULL);

	n_module_set_register(mod, 0, n_wrap_pointer(prim1), NULL);
	n_evaluator_setup(eval, mod);
	result = n_evaluator_run(eval, NULL);

	EXPECT(n_is_pointer(result));
	EXPECT(n_unwrap_pointer(result) == ptr1);

	n_module_set_register(mod, 0, n_wrap_pointer(prim2), NULL);
	n_evaluator_setup(eval, mod);
	result = n_evaluator_run(eval, NULL);

	EXPECT(n_is_pointer(result));
	EXPECT(n_unwrap_pointer(result) == ptr2);

	n_module_set_register(mod, 0, n_wrap_pointer(prim3), NULL);
	n_evaluator_setup(eval, mod);
	n_evaluator_run(eval, NULL);

	EXPECT(flag == true);

	n_primitive_destroy(prim1);
	n_primitive_destroy(prim2);
	n_primitive_destroy(prim3);

	n_module_destroy(mod);
	n_evaluator_destroy(eval);

	n_free(ptr1);
	n_free(ptr2);
}


TEST(swap_two_global_values) {
	NError error;
	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(3, 0, 5, NULL);
	NProcedure* proc;
	NValue result;
	NValue reg1, reg2;

	n_module_set_instruction(mod, 0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod, 1, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod, 2, n_op_global_set(1, 1), NULL);
	n_module_set_instruction(mod, 3, n_op_global_set(2, 0), NULL);
	n_module_set_instruction(mod, 4, n_op_return(0), NULL);

	n_module_set_register(mod, 1, n_wrap_fixnum(10588), NULL);
	n_module_set_register(mod, 2, n_wrap_fixnum(42), NULL);

	proc = n_procedure_new(mod, 0, 2, NULL);
	n_module_set_register(mod, 0, n_wrap_pointer(proc), NULL);

	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	EXPECT(n_is_equal(result, n_wrap_fixnum(10588)));

	reg1 = n_module_get_register(mod, 1, NULL);
	reg2 = n_module_get_register(mod, 2, NULL);
	EXPECT(n_is_equal(reg1, n_wrap_fixnum(42)));
	EXPECT(n_is_equal(reg2, n_wrap_fixnum(10588)));

	n_procedure_destroy(proc);
	n_module_destroy(mod);
	n_evaluator_destroy(eval);

}


/* ----- Auxiliary functions ----- */

static NValue
increment_fixnum(void*data, NValue arg, NError* error) {
	int32_t i_arg = n_unwrap_fixnum(arg);
	return n_wrap_fixnum(i_arg+1);
}


static NValue
run_value_selection_if(NValue condition, NValue v1, NValue v2) {
	NError error;
	NProcedure* proc;
	NValue result;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(4, 0, 6, NULL);

	n_module_set_instruction(mod, 0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod, 1, n_op_jump_if(0, 3), NULL);
	n_module_set_instruction(mod, 2, n_op_global_ref(1, 3), NULL);
	n_module_set_instruction(mod, 3, n_op_jump(2), NULL);
	n_module_set_instruction(mod, 4, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod, 5, n_op_return(1), NULL);

	n_module_set_register(mod, 1, condition, NULL);
	n_module_set_register(mod, 2, v1, NULL);
	n_module_set_register(mod, 3, v2, NULL);

	proc = n_procedure_new(mod, 0, 6, NULL);
	n_module_set_register(mod, 0, n_wrap_pointer(proc), NULL);

	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	return result;
}


static NValue
run_value_selection_unless(NValue condition, NValue v1, NValue v2) {
	NError error;
	NProcedure* proc;
	NValue result;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(4, 0, 6, NULL);

	n_module_set_instruction(mod, 0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod, 1, n_op_jump_unless(0, 3), NULL);
	n_module_set_instruction(mod, 2, n_op_global_ref(1, 3), NULL);
	n_module_set_instruction(mod, 3, n_op_jump(2), NULL);
	n_module_set_instruction(mod, 4, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod, 5, n_op_return(1), NULL);

	n_module_set_register(mod, 1, condition, NULL);
	n_module_set_register(mod, 2, v1, NULL);
	n_module_set_register(mod, 3, v2, NULL);

	proc = n_procedure_new(mod, 0, 6, NULL);
	n_module_set_register(mod, 0, n_wrap_pointer(proc), NULL);

	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	return result;
}


static NValue
test_constant_runner(NValue constant) {
	NError error;
	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(1, 0, 0, NULL);
	NValue result;

	n_module_set_register(mod, 0, constant, NULL);
	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);

	n_evaluator_destroy(eval);
	n_module_destroy(mod);
	return result;
}


static void
test_increment_fixnum_using_primitive(int32_t fixnum) {
	NError error;
	NValue result;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(3, 0, 4, NULL);

	n_module_set_instruction(mod, 0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod, 1, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod, 2, n_op_call_sva(0, 1, 0), NULL);
	n_module_set_instruction(mod, 3, n_op_return(0), NULL);

	{
		NPrimitive* primitive =
			n_primitive_new(increment_fixnum, NULL, NULL);

		NValue primitive_val = n_wrap_pointer(primitive);

		NProcedure* procedure = n_procedure_new(mod, 0, 2, NULL);
		NValue procedure_val = n_wrap_pointer(procedure);

		NValue fixnum_val = n_wrap_fixnum(fixnum);

		n_module_set_register(mod, 0, procedure_val, NULL);
		n_module_set_register(mod, 1, fixnum_val, NULL);
		n_module_set_register(mod, 2, primitive_val, NULL);
	}

	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	EXPECT(n_is_fixnum(result));

	EXPECT_MSG(n_unwrap_fixnum(result) == fixnum + 1,
		"Expected result of %d, but got %d instead.",
		fixnum + 1, n_unwrap_fixnum(result));

}


static NValue
toggle_bool(void* data, NValue arg, NError* error) {
	NValue result;
	bool* flag = (bool*) data;
	*flag = !(*flag);
	return result;
}


static NValue
wrap_input(void* data, NValue arg, NError* error) {
	return n_wrap_pointer(data);
}
