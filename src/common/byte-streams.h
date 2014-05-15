#ifndef __N__COMMON__BYTE_STREAM__H__
#define __N__COMMON__BYTE_STREAM__H__

#include "std-types.h"

uint8_t
n_read_byte(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
n_write_byte(uint8_t* dst, int64_t size, int64_t* index, uint8_t input);

uint16_t
n_read_uint16(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
n_write_uint16(uint8_t* dst, int64_t size, int64_t* index, uint16_t input);

int16_t
n_read_int16(uint8_t* stream, int64_t size, int64_t* index);

uint8_t
n_write_int16(uint8_t* dst, int64_t size, int64_t* index, int16_t input);

uint32_t
n_read_uint32(uint8_t* stream, int64_t size, int64_t* index);

int32_t
n_read_int32(uint8_t* stream, int64_t size, int64_t* index);

int64_t
n_read_int64(uint8_t* stream, int64_t size, int64_t* index);

uint64_t
n_read_uint64(uint8_t* stream, int64_t size, int64_t* index);

#endif
