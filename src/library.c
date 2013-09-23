#include <math.h>

#include "library.h"
#include "value.h"

nuvm_value_t nuvm_primitive_successor(void *data,
                                      nuvm_value_t* args,
                                      uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));
	if (nuvm_is_flonum(*args)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(*args) + 1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(*args) + 1);
	}
}

nuvm_value_t nuvm_primitive_predecessor(void* data,
                                        nuvm_value_t* args,
                                        uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));

	if (nuvm_is_flonum(*args)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(*args) - 1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(*args) - 1);
	}
}

nuvm_value_t nuvm_primitive_negate(void* data,
                                   nuvm_value_t* args,
                                   uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));
	if (nuvm_is_flonum(*args)) {
		return nuvm_wrap_flonum(nuvm_unwrap_flonum(*args) * -1.0);
	}
	else {
		return nuvm_wrap_fixnum(nuvm_unwrap_fixnum(*args) * -1);
	}
}

nuvm_value_t nuvm_primitive_is_positive(void* data,
                                        nuvm_value_t* args,
                                        uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));
	if (nuvm_is_flonum(*args)) {
		return (nuvm_unwrap_flonum(*args) > 0.0) ? NUVM_TRUE : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(*args) > 0) ? NUVM_TRUE : NUVM_FALSE;
	}

}

nuvm_value_t nuvm_primitive_is_negative(void* data,
                                        nuvm_value_t* args,
                                        uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));
	if (nuvm_is_flonum(*args)) {
		return (nuvm_unwrap_flonum(*args) < 0.0) ? NUVM_TRUE : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(*args) < 0) ? NUVM_TRUE : NUVM_FALSE;
	}
}

nuvm_value_t nuvm_primitive_is_zero(void* data,
                                    nuvm_value_t* args,
                                    uint8_t nargs) {
	assert(nuvm_is_flonum(*args) || nuvm_is_fixnum(*args));
	if (nuvm_is_flonum(*args)) {
		// FIXME: Yeah, this is a shitty comparison, but it won't really
		// matter right now, I just want the function here for testing
		// purposes.
		return (fabs(nuvm_unwrap_flonum(*args)) < 0.000000001) ? NUVM_TRUE
		                                                     : NUVM_FALSE;
	}
	else {
		return (nuvm_unwrap_fixnum(*args) == 0) ? NUVM_TRUE : NUVM_FALSE;
	}
}

nuvm_value_t nuvm_primitive_not(void* data,
                                nuvm_value_t* args,
                                uint8_t nargs) {
	return (!nuvm_is_equal(*args, NUVM_FALSE)) ? NUVM_TRUE : NUVM_FALSE;
}


