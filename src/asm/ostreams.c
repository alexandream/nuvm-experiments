#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "ostreams.h"

struct NOStream {
	char* buffer;
	size_t size;
	size_t cursor;
	FILE* file;
};


static size_t
available_buffer_space(NOStream* self);

static bool
can_insert_element(NOStream* self, size_t size);


NOStream*
ni_new_memory_ostream(size_t size, NError* error) {
	char* buffer = NULL;
	NOStream* result = NULL;
	uint32_t error_type = 0;

	if (size < N_OSTREAM_MIN_BUFFER_SIZE) {
		size = N_OSTREAM_MIN_BUFFER_SIZE;
	}

	buffer = (char*) malloc(sizeof(char) * size);
	if (buffer == NULL) {
		error_type = ni_a_errors.BadAllocation;
		goto cleanup;
	}

	result = (NOStream*) malloc(sizeof(NOStream));
	if(result == NULL) {
		error_type = ni_a_errors.BadAllocation;
		goto cleanup;
	}

	result->buffer = buffer;
	result->size = size;
	result->cursor = 0;
	result->file = NULL;
	return result;
cleanup:
	if (result != NULL) {
		free(result);
	}
	if (buffer != NULL) {
		free(buffer);
	}
	if (error != NULL) {
		n_error_set(error, error_type, NULL);
	}
	return NULL;
}


NOStream*
ni_new_file_ostream(const char* path, size_t buf_size, NError* error) {
	NOStream* result;
	FILE* file = NULL;
	uint32_t error_type = 0;

	n_error_reset(error);

	file = fopen(path, "wb");
	if (file == NULL) {
		error_type = ni_a_errors.IOError;
		goto cleanup;
	}

	result = ni_new_memory_ostream(buf_size, error);
	if (!n_error_ok(error)) { goto cleanup; }

	result->file = file;
	return result;
cleanup:
	if (file != NULL) {
		fclose(file);
	}

	if (error != NULL && error_type != 0) {
		n_error_set(error, error_type, NULL);
	}
	return NULL;
}


void
ni_ostream_close(NOStream* self, NError* error) {

	n_error_reset(error);

	if (self->file != NULL) {
		ni_ostream_flush(self, error);
		if (!n_error_ok(error)) return;

		if (fclose(self->file) == EOF && error != NULL) {
			n_error_set(error, ni_a_errors.IOError, NULL);
		}
		self->file = NULL;
	}
}


void
ni_ostream_flush(NOStream* self, NError* error) {
	size_t written = 0;

	if (self->file == NULL) return;
	if (self->cursor == 0) return;

	n_error_reset(error);

	written = fwrite(self->buffer, sizeof(char), self->cursor, self->file);
	if (written < self->cursor) {
		n_error_set(error, ni_a_errors.IOError, NULL);
		return;
	}
	self->cursor = 0;
}


void
ni_ostream_write_bytes(NOStream* self,
                       uint8_t* values,
                       size_t len,
                       NError* error) {
	ni_ostream_write_data(self, values, sizeof(uint8_t), len, error);
}


void
ni_ostream_write_data(NOStream* self,
                      void* mem_area,
                      size_t elem_size,
                      size_t elem_count,
                      NError* error) {
	size_t final_size = elem_size * elem_count;
	if (final_size > 0) {
		n_error_reset(error);
		/* If the current buffer can't handle the amount of data we want to
		 * insert, try to flush the buffer before inserting it. */
		if (!can_insert_element(self, final_size)) {
			ni_ostream_flush(self, error);
			if (!n_error_ok(error)) return;
		}

		/* If it still has no space, it's either an in-memory stream or the
		 * buffer is too small for it even when empty.
		 * Report a buffer size error */
		if (!can_insert_element(self, final_size)) {
			if (error != NULL) {
				n_error_set(error, ni_a_errors.BufferTooSmall, NULL);
				return;
			}
		}

		memcpy(self->buffer + self->cursor, mem_area, final_size);
		self->cursor += final_size;
	}
}


void
ni_ostream_write_uint8(NOStream* self, uint8_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(uint8_t), 1, error);
}


void
ni_ostream_write_uint16(NOStream* self, uint16_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(uint16_t), 1, error);
}


void
ni_ostream_write_int24(NOStream* self, int32_t value, NError* error) {
	uint32_t uvalue = value;
	uint8_t data[3];
	/* FIXME: This is meaningful only in little endian machines. */
	data[0] =  0xFF & uvalue;
	data[1] =  0xFF & (uvalue >> 8);
	data[2] =  0xFF & (uvalue >> 16);
}


void
ni_ostream_write_uint32(NOStream* self, uint32_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(uint32_t), 1, error);
}


void
ni_ostream_write_uint64(NOStream* self, uint64_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(uint64_t), 1, error);
}


void
ni_ostream_write_double(NOStream* self, double value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(double), 1, error);
}


void
ni_ostream_write_int8(NOStream* self, int8_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(int8_t), 1, error);
}


void
ni_ostream_write_int16(NOStream* self, int16_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(int16_t), 1, error);
}


void
ni_ostream_write_int32(NOStream* self, int32_t value, NError* error) {
	ni_ostream_write_data(self, &value, sizeof(int32_t), 1, error);
}


static size_t
available_buffer_space(NOStream* self) {
	return self->size - self->cursor;
}


static bool
can_insert_element(NOStream* self, size_t size) {
	return size <= available_buffer_space(self);
}

#ifdef TEST_ACCESSORS
size_t
nt_ostream_buffer_size(NOStream* self) {
	return self->size;
}

size_t
nt_ostream_cursor(NOStream* self) {
	return self->cursor;
}

FILE*
nt_ostream_file(NOStream* self) {
	return self->file;
}

void*
nt_ostream_buffer(NOStream* self) {
	return (void*) self->buffer;
}
#endif /* TEST_ACCESSORS */
