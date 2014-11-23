#ifndef __N__ASM__STREAMS__H__
#define __N__ASM__STREAMS__H__

#include "common/polyfills/p-stdint.h"

typedef struct NStream NStream;

void
ni_destroy_stream(NStream* self);

NStream*
ni_new_stream_from_path(const char* path);

NStream*
ni_new_stream_from_string(const char* string);

bool
ni_stream_eof(NStream* self);

uint32_t
ni_stream_length(NStream* self);

char
ni_stream_peek(NStream* self, bool* end);

char
ni_stream_read(NStream* self, bool* end);

#endif
