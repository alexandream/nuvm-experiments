#define TEST_CASE_NAME "Primitive Procedure Type"
#include "../test-case.h"

#include "nuvm.h"
#include "value.h"
#include "types/primitive.h"

BEGIN_TEST(test_module_structure_sizes) {
	fail_if(nuvm_primitive_t_size() < sizeof(nuvm_object_t));
} END_TEST


nuvm_value_t return_input(void* input, nuvm_value_t dummy) {
	return nuvm_wrap_pointer(input);
}

BEGIN_TEST(test_primitive_behaviour) {
	nuvm_value_t nil = nuvm_wrap_pointer(NULL);
	nuvm_primitive_t* primitive1 = nuvm_new_primitive(return_input, NULL);
	nuvm_value_t result1 = nuvm_primitive_call(primitive1, nil);
	
	fail_if(!nuvm_is_pointer(result1));
	fail_if(nuvm_unwrap_pointer(result1) != NULL);

	nuvm_primitive_t* primitive2 = nuvm_new_primitive(return_input, primitive1);
	nuvm_value_t result2 = nuvm_primitive_call(primitive2, nil);
	
	fail_if(!nuvm_is_pointer(result2));
	fail_if(nuvm_unwrap_pointer(result2) != primitive1);
} END_TEST



