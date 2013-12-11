#define AT_SUITE_NAME Sample_Programs
#include "../test-suite.h"

#include <string.h>

#include "error.h"
#include "value.h"

#include "evaluator.h"
#include "module.h"
#include "memory.h"

static NValue
test_constant_runner(NValue constant);


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

/* ----- Auxiliary functions ----- */
static NValue
test_constant_runner(NValue constant) {
	NError error;
	NEvaluator* eval = n_evaluator_new(NULL);
	NModule* mod = n_module_new(1, 0, NULL);
	NValue result;

	n_module_set_register(mod, 0, constant, NULL);
	n_evaluator_setup(eval, mod);

	result = n_evaluator_run(eval, &error);

	n_evaluator_destroy(eval);
	n_module_destroy(mod);
	return result;
}
