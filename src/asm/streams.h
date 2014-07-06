#ifndef __N__ASM__STREAMS__H__
#define __N__ASM__STREAMS__H__

#include "common/std-types.h"

typedef struct ni_stream_t ni_stream_t;

void
ni_destroy_stream(ni_stream_t* self);

ni_stream_t*
ni_new_stream_from_path(const char* path);

ni_stream_t*
ni_new_stream_from_string(const char* string);

bool
ni_stream_eof(ni_stream_t* self);

uint32_t
ni_stream_length(ni_stream_t* self);

char
ni_stream_peek(ni_stream_t* self, bool* end);

char
ni_stream_read(ni_stream_t* self, bool* end);

#endif
