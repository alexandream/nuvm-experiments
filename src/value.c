#include "value.h"

NValue n_wrap_pointer(void* pointer) {
	NValue result;
	result.pointer = pointer;
	return result;
}


void*  n_unwrap_pointer(NValue value) {
	return value.pointer;
}

bool n_is_equal(NValue value1, NValue value2) {
	return value1.contents == value2.contents;
}

bool n_is_pointer(NValue value) {
	return true;
}
