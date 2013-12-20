#ifndef __NUVM__OBJECTS__STRINGS_H_
#define __NUVM__OBJECTS__STRINGS_H_

#include "../error.h"
#include "../value.h"

typedef struct NString NString;

void
n_init_strings();

bool
n_is_string(NValue val);

NString*
n_string_new(NString f, void* data, NError*);

void
n_string_destroy(NString*);

#endif
