#define AT_SUITE_NAME Sample_Programs
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "value.h"

#include "evaluator.h"
#include "module.h"
#include "memory.h"

#include "objects/primitives.h"

static NValue
test_constant_runner(NValue constant);

static NValue
wrap_input(void*, NError*);

static NValue
toggle_bool(void*, NError*);


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

/* ----- Auxiliary functions ----- */
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


static NValue
wrap_input(void* data, NError* error) {
	return n_wrap_pointer(data);
}


static NValue
toggle_bool(void* data, NError* error) {
	NValue result;
	bool* flag = (bool*) data;
	*flag = !(*flag);
	return result;
}
