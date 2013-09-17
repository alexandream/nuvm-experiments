#include <math.h>

#include "library.h"
#include "value.h"

nuvm_value_t nuvm_primitive_successor(void *data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));
	if (nuvm_is_flonum(arg)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(arg) + 1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(arg) + 1);
	}
}

nuvm_value_t nuvm_primitive_predecessor(void * data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));

	if (nuvm_is_flonum(arg)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(arg) - 1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(arg) - 1);
	}
}

nuvm_value_t nuvm_primitive_negate(void * data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));
	if (nuvm_is_flonum(arg)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(arg) * -1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(arg) * -1);
	}
}

nuvm_value_t nuvm_primitive_is_positive(void * data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));
	if (nuvm_is_flonum(arg)) {
		return (nuvm_unwrap_flonum(arg) > 0.0) ? NUVM_TRUE : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(arg) > 0) ? NUVM_TRUE : NUVM_FALSE;
	}

}

nuvm_value_t nuvm_primitive_is_negative(void * data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));
	if (nuvm_is_flonum(arg)) {
		return (nuvm_unwrap_flonum(arg) < 0.0) ? NUVM_TRUE : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(arg) < 0) ? NUVM_TRUE : NUVM_FALSE;
	}
}

nuvm_value_t nuvm_primitive_is_zero(void * data, nuvm_value_t arg) {
	assert(nuvm_is_flonum(arg) || nuvm_is_fixnum(arg));
	if (nuvm_is_flonum(arg)) {
		// FIXME: Yeah, this is a shitty comparison, but it won't really
		// matter right now, I just want the function here for testing
		// purposes.
		return (fabs(nuvm_unwrap_flonum(arg)) < 0.000000001) ? NUVM_TRUE
		                                                     : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(arg) == 0) ? NUVM_TRUE : NUVM_FALSE;
	}
}

nuvm_value_t nuvm_primitive_not(void * data, nuvm_value_t arg) {
	return (!nuvm_is_equal(arg, NUVM_FALSE)) ? NUVM_TRUE : NUVM_FALSE;
}


