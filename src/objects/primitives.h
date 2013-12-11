#ifndef __NUVM__OBJECTS__PRIMITIVES_H_
#define __NUVM__OBJECTS__PRIMITIVES_H_

#include "../error.h"
#include "../value.h"

typedef struct NPrimitive NPrimitive;
typedef NValue (*NPrimitiveFunction)(void*, NError*);


void
n_init_primitives();

bool
n_is_primitive(NValue val);

NPrimitive*
n_primitive_new(NPrimitiveFunction f, void* data, NError*);

void
n_primitive_destroy(NPrimitive*);

NValue
n_primitive_call(NPrimitive*, NError*);

#endif
