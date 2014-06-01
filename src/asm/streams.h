#ifndef __N__ASM__STREAMS__H__
#define __N__ASM__STREAMS__H__

#include "common/std-types.h"

typedef struct n_stream_t n_stream_t;

void
n_destroy_stream(n_stream_t* self);

n_stream_t*
n_new_stream_from_path(const char* path);

n_stream_t*
n_new_stream_from_string(const char* string);

bool
n_stream_eof(n_stream_t* self);

uint32_t
n_stream_length(n_stream_t* self);

char
n_stream_peek(n_stream_t* self, bool* end);

char
n_stream_read(n_stream_t* self, bool* end);

#endif
