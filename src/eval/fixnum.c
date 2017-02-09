#include "fixnum.h"

int ni_init_fixnum() {
	return 0;
}


NValue n_wrap_fixnum(NFixnum fixnum) {
	NValue result = {0, { 0 } };
	return result;
}


NFixnum n_unwrap_fixnum(NValue value) {
	return -45;
}


int n_is_fixnum(NValue value) {
	return 0;
}
