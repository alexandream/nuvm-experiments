#include "values.h"

static NType _fixnum_type;
static NType _boolean_type;
static NType _unknown_type;

NValue N_TRUE;
NValue N_FALSE;

int
ni_init_values() {
    NError error = n_error_ok();

    if (ni_init_types() < 0) {
        return -1;
    }

    n_construct_type(&_fixnum_type, "nuvm.Fixnum");
    n_register_type(&_fixnum_type, &error);
    if (!n_is_ok(&error)) {
        n_destroy_error(&error);
        return -2;
    }

    n_construct_type(&_boolean_type, "nuvm.Boolean");
    n_register_type(&_boolean_type, &error);
    if (!n_is_ok(&error)) {
        n_destroy_error(&error);
        return -2;
    }

    n_construct_type(&_unknown_type, "nuvm.Unknown");
    n_register_type(&_unknown_type, &error);
    if (!n_is_ok(&error)) {
        n_destroy_error(&error);
        return -2;
    }

    N_TRUE =  0x06;
    N_FALSE = 0x02;
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
    return (value & 0X3) == 0x0;
}


int n_is_boolean(NValue value) {
    return (value & 0x3) == 0x2;
}


NType*
n_type_of(NValue value) {
    if (n_is_fixnum(value)) {
        return &_fixnum_type;
    }
    else if (n_is_boolean(value)) {
        return &_boolean_type;
    }
    return &_unknown_type;
}
