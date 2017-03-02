#ifndef N_E_VALUE_H
#define N_E_VALUE_H

#include "../common/compatibility/stdint.h"
#include "types.h"

#define N_FIXNUM_MIN -65536
#define N_FIXNUM_MAX 65535
typedef intptr_t NValue;

typedef int NFixnum;

extern NValue N_TRUE;
extern NValue N_FALSE;

int
ni_init_values(void);

NValue
n_wrap_fixnum(NFixnum fixnum);

#define n_wrap_boolean(STT) (STT? N_TRUE : N_FALSE)

#define n_unwrap_boolean(VAL) (VAL == N_TRUE ? 1 : 0)

NFixnum
n_unwrap_fixnum(NValue value);

int
n_is_fixnum(NValue value);

int
n_is_boolean(NValue value);

NType*
n_type_of(NValue value);


#define n_eq_values(LEFT, RIGHT) ((LEFT) == (RIGHT))
#endif /* N_E_VALUE_H */
