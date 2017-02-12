#ifndef N_E_FIXNUM_H
#define N_E_FIXNUM_H

typedef int NFixnum;

#include "value.h"

#define N_FIXNUM_MIN -65536
#define N_FIXNUM_MAX 65535

int
ni_init_fixnum();


NValue
n_wrap_fixnum(NFixnum fixnum);

NFixnum
n_unwrap_fixnum(NValue value);

int
n_is_fixnum(NValue value);

#endif /* N_E_FIXNUM_H */
