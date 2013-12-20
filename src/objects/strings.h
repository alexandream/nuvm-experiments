#ifndef __NUVM__OBJECTS__STRINGS_H_
#define __NUVM__OBJECTS__STRINGS_H_
#include <stddef.h>

#include "../error.h"
#include "../value.h"

typedef struct NString NString;

void
n_init_strings();

bool
n_is_string(NValue val);

NString*
n_string_new(const char* contents, NError*);

const char*
n_string_contents(NString*);

bool
n_string_is_equal(NString* self, NString* other);

uint32_t
n_string_length(NString*);
#endif
