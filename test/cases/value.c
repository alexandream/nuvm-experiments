#define TEST_CASE_NAME "Values"
#include "float.h"

#include "../test-case.h"

#include "nuvm.h"
#include "value.h"

BEGIN_TEST(test_value_has_64bit_size) {
	nuvm_value_t value;
	fail_if(sizeof(value) != sizeof(uint64_t));
	fail_if(sizeof(value) != sizeof(value.contents));
} END_TEST

BEGIN_TEST(test_value_pointer_fits_value_size) {
	nuvm_value_t value;
	fail_if(sizeof(value) != sizeof(value.pointer));
} END_TEST

BEGIN_TEST(test_value_flonum_fits_value_size) {
	nuvm_value_t value;
	fail_if(sizeof(value) != sizeof(value.flonum));
} END_TEST

BEGIN_TEST(test_value_immediate_fits_value_size) {
	nuvm_value_t value;
	fail_if(sizeof(value) != sizeof(value.immediate));
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
	nuvm_free(heap_addr);

	// Test stack address.
	nuvm_value_t value3 = nuvm_wrap_pointer(stack_addr);
	
	fail_if(!nuvm_is_pointer(value3));
	fail_if(nuvm_unwrap_pointer(value3) != stack_addr);

	// Test code address.
	nuvm_value_t value4 = nuvm_wrap_pointer(code_addr);
	
	fail_if(!nuvm_is_pointer(value4));
	fail_if(nuvm_unwrap_pointer(value4) != code_addr);

} END_TEST

BEGIN_TEST(test_is_fixnum_accepts_correct_values) {
	int32_t max = (int32_t) (INT32_MAX);
	int32_t mid = (int32_t) (0);
	int32_t min = (int32_t) (INT32_MIN);
	
	nuvm_value_t vmax = nuvm_wrap_fixnum(max);
    nuvm_value_t vmid = nuvm_wrap_fixnum(mid);
    nuvm_value_t vmin = nuvm_wrap_fixnum(min);
	
	fail_if(!nuvm_is_fixnum(vmax));

	fail_if(!nuvm_is_fixnum(vmid));

	fail_if(!nuvm_is_fixnum(vmin));
} END_TEST

BEGIN_TEST(test_is_fixnum_rejects_pointer_values) {
	uint8_t local_var;
	void* heap  = nuvm_alloc_unmanaged(sizeof(void*));
	void* stack = &local_var;
	void* code  = (void*) nuvm_alloc_unmanaged;

	nuvm_value_t vheap = nuvm_wrap_pointer(heap);
	nuvm_value_t vstack = nuvm_wrap_pointer(stack);
	nuvm_value_t vcode = nuvm_wrap_pointer(code);

	fail_if(nuvm_is_fixnum(vheap));
	fail_if(nuvm_is_fixnum(vstack));
	fail_if(nuvm_is_fixnum(vcode));
} END_TEST

BEGIN_TEST(test_wrap_unwrap_fixnum) {
	int32_t max = (int32_t) (INT32_MAX);
	int32_t mid = (int32_t) (0);
	int32_t min = (int32_t) (INT32_MIN);
	
	nuvm_value_t vmax = nuvm_wrap_fixnum(max);
    nuvm_value_t vmid = nuvm_wrap_fixnum(mid);
    nuvm_value_t vmin = nuvm_wrap_fixnum(min);

	fail_if(nuvm_unwrap_fixnum(vmin) != min);
	fail_if(nuvm_unwrap_fixnum(vmax) != max);
	fail_if(nuvm_unwrap_fixnum(vmid) != mid);
} END_TEST

BEGIN_TEST(test_is_flonum_accepts_correct_values) {
	double pos_max = DBL_MAX;
	double pos_min = DBL_MIN;
	double zero    = 0.0;
	double neg_max = -DBL_MAX;
	double neg_min = -DBL_MIN;

	nuvm_value_t vpos_max = nuvm_wrap_flonum(pos_max);
    nuvm_value_t vpos_min = nuvm_wrap_flonum(pos_min);
    nuvm_value_t vzero    = nuvm_wrap_flonum(zero);
    nuvm_value_t vneg_max = nuvm_wrap_flonum(neg_max);
    nuvm_value_t vneg_min = nuvm_wrap_flonum(neg_min);
	
	fail_if(!nuvm_is_flonum(vpos_max));
	fail_if(!nuvm_is_flonum(vpos_min));
	fail_if(!nuvm_is_flonum(vzero));
	fail_if(!nuvm_is_flonum(vneg_max));
	fail_if(!nuvm_is_flonum(vneg_min));
} END_TEST

BEGIN_TEST(test_is_flonum_rejects_pointer_values) {
	uint8_t local_var;
	void* heap  = nuvm_alloc_unmanaged(sizeof(void*));
	void* stack = &local_var;
	void* code  = (void*) nuvm_alloc_unmanaged;

	nuvm_value_t vheap = nuvm_wrap_pointer(heap);
	nuvm_value_t vstack = nuvm_wrap_pointer(stack);
	nuvm_value_t vcode = nuvm_wrap_pointer(code);

	fail_if(nuvm_is_flonum(vheap));
	fail_if(nuvm_is_flonum(vstack));
	fail_if(nuvm_is_flonum(vcode));
} END_TEST

BEGIN_TEST(test_wrap_unwrap_flonum) {
	double pos_max = DBL_MAX;
	double pos_min = DBL_MIN;
	double zero    = 0.0;
	double neg_max = -DBL_MAX;
	double neg_min = -DBL_MIN;

	nuvm_value_t vpos_max = nuvm_wrap_flonum(pos_max);
    nuvm_value_t vpos_min = nuvm_wrap_flonum(pos_min);
    nuvm_value_t vzero    = nuvm_wrap_flonum(zero);
    nuvm_value_t vneg_max = nuvm_wrap_flonum(neg_max);
    nuvm_value_t vneg_min = nuvm_wrap_flonum(neg_min);

	fail_if(nuvm_unwrap_flonum(vpos_max) !=  pos_max);
	fail_if(nuvm_unwrap_flonum(vpos_min) !=  pos_min);
	fail_if(nuvm_unwrap_flonum(vzero)    !=  zero);
	fail_if(nuvm_unwrap_flonum(vneg_max) !=  neg_max);
	fail_if(nuvm_unwrap_flonum(vneg_min) !=  neg_min);
} END_TEST

