#ifndef NHG_A_OSTREAMS
#define NHG_A_OSTREAMS

#include "common/errors.h"
#include "common/polyfills/p-stdint.h"
#define N_OSTREAM_MIN_BUFFER_SIZE 128

typedef struct NOStream NOStream;

void
ni_delete_ostream(NOStream* self);

NOStream*
ni_new_ostream_on_buffer(char* buffer, size_t buffer_size, NError* error);

NOStream*
ni_new_memory_ostream(size_t size, NError* error);

NOStream*
ni_new_file_ostream(const char* path, size_t bufsize, NError *error);

void
ni_ostream_write_uint8(NOStream* self, uint8_t value, NError* error);

void
ni_ostream_write_uint16(NOStream* self, uint16_t value, NError* error);

void
ni_ostream_write_uint32(NOStream* self, uint32_t value, NError* error);

void
ni_ostream_write_uint64(NOStream* self, uint64_t value, NError* error);

void
ni_ostream_write_double(NOStream* self, double value, NError* error);

void
ni_ostream_write_int8(NOStream* self, int8_t value, NError* error);

void
ni_ostream_write_int16(NOStream* self, int16_t value, NError* error);

void
ni_ostream_write_int24(NOStream* self, int32_t value, NError* error);

void
ni_ostream_write_int32(NOStream* self, int32_t value, NError* error);

void
ni_ostream_write_bytes(NOStream* self,
                       uint8_t* values,
                       size_t len,
                       NError* error);

void
ni_ostream_write_data(NOStream* self,
                      void* mem_area,
                      size_t elem_size,
                      size_t elem_count,
                      NError* error);

void
ni_ostream_close(NOStream* self, NError* error);

void
ni_ostream_flush(NOStream* self, NError* error);


#ifdef TEST_ACCESSORS

#include <stdio.h>

void*
nt_ostream_buffer(NOStream* self);

size_t
nt_ostream_buffer_size(NOStream* self);

size_t
nt_ostream_cursor(NOStream* self);

FILE*
nt_ostream_file(NOStream* self);

#endif /* TEST_ACCESSORS */

#endif /* NHG_A_OSTREAMS */
