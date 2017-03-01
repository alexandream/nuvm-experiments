#include "fixnum.h"

static NType _fixnum_type;

int
ni_init_fixnum() {
    NError error = n_error_ok();

    n_construct_type(&_fixnum_type, "nuvm.Fixnum");
    n_register_type(&_fixnum_type, &error);
    if (!n_is_ok(&error)) {
        n_destroy_error(&error);
        return -1;
    }

    return 0;
}


NValue
n_wrap_fixnum(NFixnum fixnum) {
	return fixnum << 2;
}


NFixnum
n_unwrap_fixnum(NValue value) {
    return value >> 2;
}


int
n_is_fixnum(NValue value) {
    return (value & ~0X3) == value;
}
