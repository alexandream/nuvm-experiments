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
add_many_fixnums(void* data, NValue* args, uint8_t n_args, NError*);

static NValue
fixnum_less_than(void* data, NValue* args, uint8_t n_args, NError* error);

static NValue
increment_fixnum(void*data, NValue* args, uint8_t nargs, NError*);

static void
run_less_than_with_primitives(int32_t a, int32_t b);

static void
run_sum_of_fixnums(int32_t a1, int32_t a2, int32_t a3, int32_t a4,
                   int32_t a5, int32_t a6, int32_t a7);

static NValue
run_value_selection_if(NValue condition, NValue v1, NValue v2);

static NValue
run_value_selection_unless(NValue condition, NValue v1, NValue v2);

static NValue
test_constant_runner(NValue constant);

static void
test_identity_procedure(NValue value, uint8_t nlocals);

static void
test_increment_fixnums_on_many_proceures(int32_t x, int32_t y, int32_t z);

static void
test_increment_fixnum_using_primitive(int32_t fixnum);

static void
test_max_procedure(int32_t a, int32_t b);

static NValue
toggle_bool(void*, NValue* args, uint8_t nargs, NError*);

static NValue
wrap_input(void*, NValue* arg, uint8_t nargs, NError*);



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


TEST(increment_fixnums_on_multiple_procedures) {
	test_increment_fixnums_on_many_proceures(1, 2, 3);
	test_increment_fixnums_on_many_proceures(234242, 23456212, 66546113);
}


TEST(max_procedure) {
	test_max_procedure(10, 2);
	test_max_procedure(1, 2);
	test_max_procedure(-431, 2);
	test_max_procedure(121, 122);
}

TEST(less_than_with_primitives) {
	run_less_than_with_primitives(1, 2);
	run_less_than_with_primitives(10, 2);
	run_less_than_with_primitives(-431, 2);
	run_less_than_with_primitives(121, 122);
}


TEST(identity_procedure) {
	test_identity_procedure(N_UNDEFINED, 0);
	test_identity_procedure(N_UNDEFINED, 30);
	test_identity_procedure(N_TRUE, 0);
	test_identity_procedure(N_TRUE, 200);
	test_identity_procedure(N_FALSE, 0);
	test_identity_procedure(N_FALSE, 200);
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

	n_module_destroy(mod);
	n_evaluator_destroy(eval);

	n_free(ptr1);
	n_free(ptr2);
}


TEST(sum_of_many_fixnums) {
	run_sum_of_fixnums(1, 2, 3, 4, 5, 6, 7);
	run_sum_of_fixnums(11, 12, 13, -14, -15, 16, 17);
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

	n_module_destroy(mod);
	n_evaluator_destroy(eval);

}


/* ----- Auxiliary functions ----- */
static NValue
add_many_fixnums(void* data, NValue* args, uint8_t n_args, NError* error) {
	int32_t sum = 0;
	int i;
	for (i = 0; i < n_args; i++) {
		sum += n_unwrap_fixnum(args[i]);
	}
	return n_wrap_fixnum(sum);
}


static NValue
fixnum_less_than(void* data, NValue* args, uint8_t n_args, NError* error) {
	int32_t arg0;
	int32_t arg1;

	assert(n_args == 2);
	arg0 = n_unwrap_fixnum(args[0]);
	arg1 = n_unwrap_fixnum(args[1]);
	return (arg0 < arg1) ? N_TRUE : N_FALSE;
}


static NValue
increment_fixnum(void*data, NValue* args, uint8_t nargs, NError* error) {
	int32_t i_arg = n_unwrap_fixnum(*args);
	return n_wrap_fixnum(i_arg+1);
}


static void
run_less_than_with_primitives(int32_t a, int32_t b) {
	NValue expected_result = a < b ? N_TRUE : N_FALSE;
	NError error;
	NPrimitive* less_than;
	NProcedure* entry;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(4, 0, 6, NULL);

	n_module_set_instruction(mod,  0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod,  1, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod,  2, n_op_global_ref(2, 3), NULL);
	n_module_set_instruction(mod,  3, n_op_call(3, 0, 2), NULL);

	n_module_set_instruction(mod,  4, n_pack_op_arguments(1, 2, 0, 0), NULL);
	n_module_set_instruction(mod,  5, n_op_return(3), NULL);

	less_than = n_primitive_new(fixnum_less_than, NULL, NULL);

	entry = n_procedure_new(mod, 0, 4, NULL);


	n_module_set_register(mod, 0, n_wrap_pointer(entry), NULL);
	n_module_set_register(mod, 1, n_wrap_pointer(less_than), NULL);
	n_module_set_register(mod, 2, n_wrap_fixnum(a), NULL);
	n_module_set_register(mod, 3, n_wrap_fixnum(b), NULL);

	n_evaluator_setup(eval, mod);
	{
		char *expected_str = "[Unknown value]";
		char *result_str = "[Unknown value]";

		NValue result = n_evaluator_run(eval, &error);
		EXPECT_MSG(error.code == N_E_OK,
			"Expected error code %u, got %u.", N_E_OK, error.code);
		EXPECT(n_is_boolean(result));

		if (n_is_equal(expected_result, N_TRUE)) {
			expected_str = "True";
		}
		else if (n_is_equal(expected_result, N_FALSE)) {
			expected_str = "False";
		}

		if (n_is_equal(result, N_TRUE)) {
			result_str = "True";
		}
		else if (n_is_equal(result, N_FALSE)) {
			result_str = "False";
		}
		EXPECT_MSG(n_is_equal(expected_result, result),
				"Expected result of %s, but got %s instread.",
				expected_str, result_str);
	}
}


static void run_sum_of_fixnums(int32_t a1,
                               int32_t a2,
                               int32_t a3,
                               int32_t a4,
                               int32_t a5,
                               int32_t a6,
                               int32_t a7) {
	NError error;
	NPrimitive* primitive;
	NProcedure* entry;
	NValue result;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(9, 0, 12, NULL);

	n_module_set_instruction(mod,  0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod,  1, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod,  2, n_op_global_ref(2, 3), NULL);
	n_module_set_instruction(mod,  3, n_op_global_ref(3, 4), NULL);
	n_module_set_instruction(mod,  4, n_op_global_ref(4, 5), NULL);
	n_module_set_instruction(mod,  5, n_op_global_ref(5, 6), NULL);
	n_module_set_instruction(mod,  6, n_op_global_ref(6, 7), NULL);
	n_module_set_instruction(mod,  7, n_op_global_ref(7, 8), NULL);

	n_module_set_instruction(mod,  8, n_op_call(8, 0, 7), NULL);
    n_module_set_instruction(mod,  9,
			n_pack_op_arguments(1, 2, 3, 4), NULL);

    n_module_set_instruction(mod, 10,
			n_pack_op_arguments(5, 6, 7, 0), NULL);

	n_module_set_instruction(mod, 11, n_op_return(8), NULL);

	primitive = n_primitive_new(add_many_fixnums, NULL, NULL);

	entry = n_procedure_new(mod, 0, 9, NULL);

	n_module_set_register(mod, 0, n_wrap_pointer(entry), NULL);
	n_module_set_register(mod, 1, n_wrap_pointer(primitive), NULL);
	n_module_set_register(mod, 2, n_wrap_fixnum(a1), NULL);
	n_module_set_register(mod, 3, n_wrap_fixnum(a2), NULL);
	n_module_set_register(mod, 4, n_wrap_fixnum(a3), NULL);
	n_module_set_register(mod, 5, n_wrap_fixnum(a4), NULL);
	n_module_set_register(mod, 6, n_wrap_fixnum(a5), NULL);
	n_module_set_register(mod, 7, n_wrap_fixnum(a6), NULL);
	n_module_set_register(mod, 8, n_wrap_fixnum(a7), NULL);

	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	EXPECT(n_is_fixnum(result));

	{
		int32_t expected_result = a1 + a2 + a3 + a4 + a5 + a6 + a7;
		EXPECT_MSG(n_unwrap_fixnum(result) == expected_result,
			"Expected result of %d, but got %d instead.",
			expected_result, result);
	}
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
test_identity_procedure(NValue value, uint8_t nlocals) {
	NError error;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(3, 0, 10, NULL);

	NProcedure* main;
	NProcedure* identity;

	NValue result;

	/* Body for "main" procedure. */
	n_module_set_instruction(mod, 0, n_op_global_ref(0, 2), NULL);
	n_module_set_instruction(mod, 1, n_op_global_ref(1, 1), NULL);
	n_module_set_instruction(mod, 2, n_op_call_sva(2, 1, 0), NULL);
	n_module_set_instruction(mod, 3, n_op_return(2), NULL);

	/* Body for "identity" procedure. */
	n_module_set_instruction(mod, 4, n_op_return(nlocals), NULL);

	main = n_procedure_new(mod, 0, 3, NULL);
	identity = n_procedure_new(mod, 4, nlocals, NULL);

	n_module_set_register(mod, 0, n_wrap_pointer(main), NULL);
	n_module_set_register(mod, 1, n_wrap_pointer(identity), NULL);
	n_module_set_register(mod, 2, value, NULL);

	n_evaluator_setup(eval, mod);
	result = n_evaluator_run(eval, &error);
	EXPECT_MSG(error.code == N_E_OK,
		"Running identity procedure with input value 0x%016lX and number "
		"of locals %u yielded wrong error message. Expected %u, got %u.",
		value.contents, nlocals, N_E_OK, error.code);

	EXPECT_MSG(n_is_equal(result, value),
		"Running identity procedure with input value 0x%016lX and number "
		"of locals %u yielded wrong output. Expected 0x%016lX got 0x%016lX.",
		value.contents, nlocals, value.contents, result.contents);
}


static void
test_increment_fixnums_on_many_proceures(int32_t x, int32_t y, int32_t z) {
	NError error;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(7, 0, 30, NULL);

	NValue x_val = n_wrap_fixnum(x);
	NValue y_val = n_wrap_fixnum(y);
	NValue z_val = n_wrap_fixnum(z);

	NPrimitive* primitive;

	NProcedure *inc_reg3, *inc_reg4, *inc_reg5;
	NValue result;

	/* Instructions for inc_reg3 */
	n_module_set_instruction(mod, 0, n_op_global_ref(0, 6), NULL);
	n_module_set_instruction(mod, 1, n_op_global_ref(1, 3), NULL);
	n_module_set_instruction(mod, 2, n_op_call_sva(1, 0, 1), NULL);
	n_module_set_instruction(mod, 3, n_op_global_set(3, 1), NULL);
	/* Call next (inc_reg4) procedure */
	n_module_set_instruction(mod, 4, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod, 5, n_op_call_sva(1, 0, 1), NULL);
	/* Increment the result from the call before returning */
	n_module_set_instruction(mod, 6, n_op_global_ref(0, 6), NULL);
	n_module_set_instruction(mod, 7, n_op_call_sva(1, 0, 1), NULL);
	n_module_set_instruction(mod, 8, n_op_return(1), NULL);
	/* Explanation for the code below:
	 * first procedure, inc_reg3, gets the register number 3 and
	 * incrmeents it, saving it again on reg #3, then it calls
	 * inc_reg4 and returns the return value of inc_reg4 + 1;
	 *
	 * inc_reg4 gets the register #4 and increments it, saving it
	 * again on reg #4, then it calls inc_reg5 and returns the
	 * return value of inc_reg5;
	 *
	 * inc_reg5 gets the register #5, increments its value and
	 * returns it.
	 *
	 * In the end, the first procedure receives the value returned
	 * by inc_reg5 and increments it, so the expected final result is
	 * the value of register #5 (z) + 2. */

	/* Instructions for inc_reg4 */
	n_module_set_instruction(mod, 9, n_op_global_ref(0, 6), NULL);
	n_module_set_instruction(mod, 10, n_op_global_ref(1, 4), NULL);
	n_module_set_instruction(mod, 11, n_op_call_sva(1, 0, 1), NULL);
	n_module_set_instruction(mod, 12, n_op_global_set(4, 1), NULL);

	/* Call next (inc_reg5) procedure */
	n_module_set_instruction(mod, 13, n_op_global_ref(0, 2), NULL);
	n_module_set_instruction(mod, 14, n_op_call_sva(1, 0, 1), NULL);
	n_module_set_instruction(mod, 15, n_op_return(1), NULL);

	/* Instructions for inc_reg5 */
	n_module_set_instruction(mod, 16, n_op_global_ref(0, 6), NULL);
	n_module_set_instruction(mod, 17, n_op_global_ref(1, 5), NULL);
	n_module_set_instruction(mod, 18, n_op_call_sva(1, 0, 1), NULL);
	n_module_set_instruction(mod, 19, n_op_global_set(5, 1), NULL);
	n_module_set_instruction(mod, 20, n_op_return(1), NULL);

	primitive = n_primitive_new(increment_fixnum, NULL, NULL);

	inc_reg3 = n_procedure_new(mod, 0, 2, NULL);
	inc_reg4 = n_procedure_new(mod, 9, 2, NULL);
	inc_reg5 = n_procedure_new(mod, 16, 2, NULL);

	n_module_set_register(mod, 0, n_wrap_pointer(inc_reg3), NULL);
	n_module_set_register(mod, 1, n_wrap_pointer(inc_reg4), NULL);
	n_module_set_register(mod, 2, n_wrap_pointer(inc_reg5), NULL);
	n_module_set_register(mod, 3, x_val, NULL);
	n_module_set_register(mod, 4, y_val, NULL);
	n_module_set_register(mod, 5, z_val, NULL);
	n_module_set_register(mod, 6, n_wrap_pointer(primitive), NULL);

	n_evaluator_setup(eval, mod);
	result = n_evaluator_run(eval, &error);
	EXPECT(error.code == N_E_OK);
	EXPECT(n_is_fixnum(result));

	EXPECT_MSG(n_unwrap_fixnum(result) == z + 2,
		"Expected result of %d, but got %d instead.",
		z + 2, n_unwrap_fixnum(result));
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


static void
test_max_procedure(int32_t a, int32_t b) {
	NError error;

	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(5, 0, 15, NULL);

	NProcedure* main;
	NProcedure* max;
	NPrimitive* less_than;

	NValue expected = n_wrap_fixnum(a < b ? a : b);
	NValue result;

	/* Body for "main" procedure. */
	n_module_set_instruction(mod,  0, n_op_global_ref(0, 1), NULL);
	n_module_set_instruction(mod,  1, n_op_global_ref(1, 2), NULL);
	n_module_set_instruction(mod,  2, n_op_global_ref(2, 3), NULL);
	n_module_set_instruction(mod,  3, n_op_call(3, 2, 2), NULL);
	n_module_set_instruction(mod,  4, n_pack_op_arguments(0, 1, 9, 9), NULL);
	n_module_set_instruction(mod,  5, n_op_return(3), NULL);

	/* Body for "max" procedure. */
	n_module_set_instruction(mod,  6, n_op_global_ref(0, 4), NULL);
	n_module_set_instruction(mod,  7, n_op_call(1, 0, 2), NULL);
	n_module_set_instruction(mod,  8, n_pack_op_arguments(2, 3, 9, 9), NULL);
	n_module_set_instruction(mod,  9, n_op_jump_if(1, +2), NULL);
	n_module_set_instruction(mod, 10, n_op_return(3), NULL);
	n_module_set_instruction(mod, 11, n_op_return(2), NULL);

	main = n_procedure_new(mod, 0, 4, NULL);
	max = n_procedure_new(mod, 6, 2, NULL);
	less_than = n_primitive_new(fixnum_less_than, NULL, NULL);

	n_module_set_register(mod, 0, n_wrap_pointer(main), NULL);
	n_module_set_register(mod, 1, n_wrap_fixnum(a), NULL);
	n_module_set_register(mod, 2, n_wrap_fixnum(b), NULL);
	n_module_set_register(mod, 3, n_wrap_pointer(max), NULL);
	n_module_set_register(mod, 4, n_wrap_pointer(less_than), NULL);


	n_evaluator_setup(eval, mod);
	result = n_evaluator_run(eval, &error);
	EXPECT_MSG(error.code == N_E_OK,
		"Running max procedure with inputs %d & %d yielded wrong "
		"error code: expected %u, got %u.",
		a, b, N_E_OK, error.code);

	EXPECT_MSG(n_is_fixnum(result),
		"Running max procedure with inputs %d & %d yielded result "
		"that fails to be recognized as fixnum.",
		a, b);

	EXPECT_MSG(n_is_equal(result, expected),
		"Running max procedure with inputs %d & %d yielded wrong result: "
		"expected %d, got %d.",
		n_unwrap_fixnum(expected), n_unwrap_fixnum(result));
}


static NValue
toggle_bool(void* data, NValue* arg, uint8_t nargs, NError* error) {
	NValue result;
	bool* flag = (bool*) data;
	*flag = !(*flag);
	return result;
}


static NValue
wrap_input(void* data, NValue* args, uint8_t nargs, NError* error) {
	return n_wrap_pointer(data);
}
