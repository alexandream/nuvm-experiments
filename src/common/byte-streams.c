#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "polyfills/p-stdint.h"

#include "byte-streams.h"

static void
_safe_read(void* dst, uint8_t* src, int64_t size, int64_t* i, size_t part_size);


uint8_t
ni_read_byte(uint8_t* stream, int64_t size, int64_t* i) {
	uint8_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(uint8_t));
	return result;
}


uint8_t
ni_write_byte(uint8_t* dst, int64_t size, int64_t* index, uint8_t input) {
	dst[*index] = input;
	*index += sizeof(uint8_t);
	return sizeof(uint8_t);
}


uint16_t
ni_read_uint16(uint8_t* stream, int64_t size, int64_t* i) {
	uint16_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(uint16_t));
	return result;
}


uint8_t
ni_write_uint16(uint8_t* dst, int64_t size, int64_t* index, uint16_t input) {
	memcpy(dst + *index, (void*) &input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


int16_t
ni_read_int16(uint8_t* stream, int64_t size, int64_t* i) {
	int16_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(int16_t));
	return result;
}


uint8_t
ni_write_int16(uint8_t* dst, int64_t size, int64_t* index, int16_t input) {
	memcpy(dst + *index, (void*) &input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


int32_t
ni_read_int32(uint8_t* stream, int64_t size, int64_t* i) {
	int32_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(result));
	return result;
}


uint8_t
ni_write_int32(uint8_t* dst, int64_t size, int64_t* index, int32_t input) {
	memcpy(dst + *index, (void*)&input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


uint32_t
ni_read_uint32(uint8_t* stream, int64_t size, int64_t* i) {
	uint32_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(result));
	return result;
}


uint8_t
ni_write_uint32(uint8_t* dst, int64_t size, int64_t* index, uint32_t input) {
	memcpy(dst + *index, (void*)&input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


int64_t
ni_read_int64(uint8_t* stream, int64_t size, int64_t* i) {
	int64_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(result));
	return result;
}


uint8_t
ni_write_int64(uint8_t* dst, int64_t size, int64_t* index, int64_t input) {
	memcpy(dst + *index, (void*)&input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


uint64_t
ni_read_uint64(uint8_t* stream, int64_t size, int64_t* i) {
	uint64_t result;
	_safe_read((void*) &result, (void*) stream, size, i, sizeof(result));
	return result;
}


uint8_t
ni_write_uint64(uint8_t* dst, int64_t size, int64_t* index, uint64_t input) {
	memcpy(dst + *index, (void*)&input, sizeof(input));
	*index += sizeof(input);
	return sizeof(input);
}


/* Copies /part_size/ bytes from the byte array /src/ into /dst/, if /src/ has
 * enough bytes to fulfill the copy. Otherwise, the index /i/ is set to -1 and
 * no copy is performed. */
static void
_safe_read(void* dst, uint8_t* src, int64_t size, int64_t* i, size_t part_size) {
	bool is_safe;
	assert(size >= 0 && *i >= 0);

	/* FIXME: Check for overflow */
	is_safe = (*i + part_size) <= size;
	if (is_safe) {
		memcpy(dst, src + *i, part_size);
		*i += part_size;
	}
	else {
		*i = -1;
	}
}
