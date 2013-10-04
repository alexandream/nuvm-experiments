#include <math.h>

#include "types/strings.h"

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

nuvm_value_t nuvm_primitive_fixnum_add(void* data,
                                       nuvm_value_t* args,
                                       uint8_t nargs) {
	int32_t result = 0;
	for (int i = 0; i < nargs; i++) {
		assert(nuvm_is_fixnum(args[i]));
		result += nuvm_unwrap_fixnum(args[i]);
	}
	return nuvm_wrap_fixnum(result);
}
nuvm_value_t nuvm_primitive_flonum_add(void* data,
                                       nuvm_value_t* args,
                                       uint8_t nargs) {
	int32_t result = 0;
	for (int i = 0; i < nargs; i++) {
		assert(nuvm_is_flonum(args[i]));
		result += nuvm_unwrap_flonum(args[i]);
	}
	return nuvm_wrap_flonum(result);
}

nuvm_value_t nuvm_primitive_not(void* data,
                                nuvm_value_t* args,
                                uint8_t nargs) {
	return (!nuvm_is_equal(*args, NUVM_FALSE)) ? NUVM_TRUE : NUVM_FALSE;
}


nuvm_value_t nuvm_primitive_and(void* data,
                                nuvm_value_t* args,
                                uint8_t nargs) {
	bool result = true;
	for (int i = 0; result == true && i < nargs; i++) {
		assert(nuvm_is_boolean(args[i]));
		result = nuvm_is_equal(args[i], NUVM_TRUE);
	}
	return result ? NUVM_TRUE : NUVM_FALSE;
}
nuvm_value_t nuvm_primitive_or(void* data,
                              nuvm_value_t* args,
                              uint8_t nargs) {
	bool result = false;
	for (int i = 0; result == false && i < nargs; i++) {
		assert(nuvm_is_boolean(args[i]));
		result = nuvm_is_equal(args[i], NUVM_TRUE);
	}
	return result ? NUVM_TRUE : NUVM_FALSE;
}
nuvm_value_t nuvm_primitive_string_eq(void* data,
                                      nuvm_value_t* args,
                                      uint8_t nargs) {
	assert(nargs == 2);
	assert(nuvm_is_string(args[0]) && nuvm_is_string(args[1]));
	nuvm_string_t* arg0 = nuvm_unwrap_pointer(args[0]);
	nuvm_string_t* arg1 = nuvm_unwrap_pointer(args[1]);

	bool result = nuvm_string_is_equal(arg0, arg1);
	return result ? NUVM_TRUE : NUVM_FALSE;
}
nuvm_value_t nuvm_primitive_string_len(void* data,
                                       nuvm_value_t* args,
                                       uint8_t nargs) {
	assert(nargs == 1);
	assert(nuvm_is_string(args[0]));

	nuvm_string_t* arg0 = nuvm_unwrap_pointer(args[0]);

	// FIXME: The representation for fixnums can't encode the whole possible
	// range of the following value.
	int32_t result = (int32_t) nuvm_string_length(arg0);
	return nuvm_wrap_fixnum(result);
}
nuvm_value_t nuvm_primitive_is_same(void* data,
                                    nuvm_value_t* args,
                                    uint8_t nargs) {
	assert(nargs == 2);
	return nuvm_is_equal(args[0], args[1]) ? NUVM_TRUE : NUVM_FALSE;
}
