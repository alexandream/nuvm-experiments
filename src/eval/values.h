#ifndef N_E_VALUE_H
#define N_E_VALUE_H

#include "../common/compatibility/stdint.h"
#include "types.h"

#define N_FIXNUM_MIN -65536
#define N_FIXNUM_MAX 65535
typedef intptr_t NValue;

typedef int NFixnum;

int
ni_init_values(void);

NValue
n_wrap_fixnum(NFixnum fixnum);

NFixnum
n_unwrap_fixnum(NValue value);

int
n_is_fixnum(NValue value);


#endif /* N_E_VALUE_H */
