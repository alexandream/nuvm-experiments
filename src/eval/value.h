#ifndef N_E_VALUE_H
#define N_E_VALUE_H

#include "types.h"

typedef struct NValue NValue;

union NValueAlternatives {
	void* pointer;
    long  fixnum;
};


struct NValue {
	NType* type;
	union NValueAlternatives alt;
};


#define n_type_of(VALUE) (VALUE.type)

#endif /* N_E_VALUE_H */
