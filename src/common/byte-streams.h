#ifndef NHG_C_BYTE_STREAM_
#define NHG_C_BYTE_STREAM_

#include "polyfills/p-stdint.h"

uint8_t
ni_read_byte(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_byte(uint8_t* dst, int64_t size, int64_t* index, uint8_t input);

uint16_t
ni_read_uint16(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_uint16(uint8_t* dst, int64_t size, int64_t* index, uint16_t input);

int16_t
ni_read_int16(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_int16(uint8_t* dst, int64_t size, int64_t* index, int16_t input);

uint32_t
ni_read_uint32(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_uint32(uint8_t* dst, int64_t size, int64_t* index, uint32_t input);

int32_t
ni_read_int32(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_int32(uint8_t* dst, int64_t size, int64_t* index, int32_t input);

int64_t
ni_read_int64(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
ni_write_int64(uint8_t* dst, int64_t size, int64_t* index, int64_t input);

uint64_t
ni_read_uint64(uint8_t* stream, int64_t size, int64_t* index);

#endif
