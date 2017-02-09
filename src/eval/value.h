#ifndef N_E_VALUE_H
#define N_E_VALUE_H

#include "types.h"
#include "fixnum.h"

typedef struct NValue NValue;

union NValueAlternatives {
	void* pointer;
};


struct NValue {
	NType* type;
	union NValueAlternatives alt;
};


#define n_type_of(VALUE) (VALUE.type)

#endif /* N_E_VALUE_H */
