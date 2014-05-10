#include <assert.h>
#include <string.h>

#include "std-types.h"

#include "byte-streams.h"

static void
safe_read(void* dst, void* src, int64_t size, int64_t* i, size_t part_size);


uint8_t
n_read_byte(uint8_t* stream, int64_t size, int64_t* i) {
	uint8_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(uint8_t));
	return result;
}


uint16_t
n_read_uint16(uint8_t* stream, int64_t size, int64_t* i) {
	uint16_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(uint16_t));
	return result;
}


int16_t
n_read_int16(uint8_t* stream, int64_t size, int64_t* i) {
	int16_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(int16_t));
	return result;
}


uint32_t
n_read_uint32(uint8_t* stream, int64_t size, int64_t* i) {
	uint32_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(uint32_t));
	return result;
}


int32_t
n_read_int32(uint8_t* stream, int64_t size, int64_t* i) {
	int32_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(int32_t));
	return result;
}


int64_t
n_read_int64(uint8_t* stream, int64_t size, int64_t* i) {
	uint64_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(uint64_t));
	return result;
}


uint64_t
n_read_uint64(uint8_t* stream, int64_t size, int64_t* i) {
	int64_t result;
	safe_read((void*) &result, (void*) stream, size, i, sizeof(int64_t));
	return result;
}


/* Copies /part_size/ bytes from the byte array /src/ into /dst/, if /src/ has
 * enough bytes to fulfill the copy. Otherwise, the index /i/ is set to -1 and
 * no copy is performed. */
static void
safe_read(void* dst, void* src, int64_t size, int64_t* i, size_t part_size) {
	bool is_safe;
	assert(size >= 0 && *i >= 0);

	/* FIXME: Check for overflow */
	is_safe = (*i + part_size) <= size;
	if (is_safe) {
		memcpy(dst, src, part_size);
		*i += part_size;
	}
	else {
		*i = -1;
	}
}

