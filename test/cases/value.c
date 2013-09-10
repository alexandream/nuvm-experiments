#define TEST_CASE_NAME "Values"
#include "../test-case.h"

#include "nuvm.h"
#include "value.h"

BEGIN_TEST(test_value_structure_sizes) {
	nuvm_value_t value;

	fail_if(sizeof(value.pointer) != 8);
	fail_if(sizeof(value) != 8);
} END_TEST


BEGIN_TEST(test_pointer_values) {
	uint8_t local_var;
	void* heap_addr  = nuvm_alloc_unmanaged(sizeof(void*));
	void* stack_addr = &local_var;
	void* code_addr  = (void*) nuvm_alloc_unmanaged;
	// Test null pointer.
	nuvm_value_t value1 = nuvm_wrap_pointer(NULL);

	fail_if(!nuvm_is_pointer(value1));
	fail_if(nuvm_unwrap_pointer(value1) != NULL);

	// Test unmanaged heap address.
	nuvm_value_t value2 = nuvm_wrap_pointer(heap_addr);
	
	fail_if(!nuvm_is_pointer(value2));
	fail_if(nuvm_unwrap_pointer(value2) != heap_addr);

	// Test stack address.
	nuvm_value_t value3 = nuvm_wrap_pointer(stack_addr);
	
	fail_if(!nuvm_is_pointer(value3));
	fail_if(nuvm_unwrap_pointer(value3) != stack_addr);

	// Test code address.
	nuvm_value_t value4 = nuvm_wrap_pointer(code_addr);
	
	fail_if(!nuvm_is_pointer(value4));
	fail_if(nuvm_unwrap_pointer(value4) != code_addr);

} END_TEST



