#include "util/common.h"

#include "types/primitive.h"

#include "evaluator.h"
#include "nuvm.h"

#define STACK_SIZE 8192

struct nuvm_evaluator_t {
	int dummy;
};

// Static prototypes.
static void
_construct(nuvm_evaluator_t* self);


// Function implementations.
nuvm_evaluator_t* nuvm_new_evaluator() {
	size_t size = sizeof(nuvm_evaluator_t);
	nuvm_evaluator_t* self = (nuvm_evaluator_t*) nuvm_alloc_unmanaged(size);

	_construct(self);
	return self;
}
void nuvm_destroy_evaluator(nuvm_evaluator_t* self) {
	nuvm_free(self);
}

nuvm_value_t nuvm_evaluator_run_module(nuvm_evaluator_t* self,
                                       nuvm_module_t* module) {
	nuvm_value_t entry_point = nuvm_module_load_entry_point(module);
	assert(nuvm_typeof(entry_point) == NUVM_PRIMITIVE_T_TYPE());
	nuvm_primitive_t* primitive =
		(nuvm_primitive_t*) nuvm_unwrap_pointer(entry_point);

	return nuvm_primitive_call(primitive);
}


// Static function implementations.
static void
_construct(nuvm_evaluator_t* self) {

}

