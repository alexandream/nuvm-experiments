#ifndef __N__ASM__STREAMS__H__
#define __N__ASM__STREAMS__H__

#include "common/errors.h"
#include "common/polyfills/p-stdint.h"

typedef struct NIStream NIStream;

void
ni_destroy_istream(NIStream* self);

NIStream*
ni_new_istream_from_path(const char* path, NError* error);

NIStream*
ni_new_istream_from_string(const char* string);

bool
ni_istream_eof(NIStream* self);

char
ni_istream_peek(NIStream* self, bool* end);

char
ni_istream_read(NIStream* self, bool* end);

#endif
