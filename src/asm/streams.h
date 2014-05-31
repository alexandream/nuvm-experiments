#ifndef __N__ASM__STREAMS__H__
#define __N__ASM__STREAMS__H__

typedef struct n_stream_t n_stream_t;

n_stream_t*
n_new_string_stream(const char* string);

void n_destroy_stream(n_stream_t* self);
#endif
