#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/polyfills/strdup.h"

#include "streams.h"

struct NStream {
	char* buffer;
	int32_t size;
	int32_t cursor;
	bool eof;
};

void
ni_destroy_stream(NStream* self) {
	free(self);
}


NStream*
ni_new_stream_from_path(const char* path) {
	/* TODO: Add error reporting and detailed error checking. */
	int status;
	int32_t file_size, i;
	char* buffer = NULL;
	NStream* result = NULL;

	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		goto cleanup;
	}

	status = fseek(file, 0, SEEK_END);
	if (status != 0) {
		goto cleanup;
	}

	file_size = ftell(file);
	if (file_size < 0) {
		goto cleanup;
	}

	status = fseek(file, 0, SEEK_SET);
	if (status != 0) {
		goto cleanup;
	}

	result = malloc(sizeof(NStream));
	if (result == NULL) {
		goto cleanup;
	}

	if (file_size > 0) {
		buffer = malloc(sizeof(char) * (file_size + 1));
		if (buffer == NULL) {
			goto cleanup;
		}

		i = 0;
		while (!feof(file)) {
			size_t n = fread(buffer + i, sizeof(char), file_size - i + 1, file);
			if (ferror(file)) {
				goto cleanup;
			}
			i += n;
		}
		buffer[file_size] = '\0';
	}
	result->buffer = buffer;
	result->size = file_size;
	result->cursor = 0;
	result->eof = file_size == 0;
	return result;

cleanup:
	if (file != NULL) {
		fclose(file);
	}
	if (result != NULL) {
		free(result);
	}
	if (buffer != NULL) {
		free(buffer);
	}

	return NULL;
}


NStream*
ni_new_stream_from_string(const char* string) {
	/* TODO: Add error reporting and detailed error checking. */
	size_t length = strlen(string);
	NStream* result = NULL;
	char* buffer = NULL;
	if (length > INT32_MAX) {
		return NULL;
	}

	result = malloc(sizeof(NStream));
	if (result == NULL) {
		return NULL;
	}

	buffer = strdup(string);
	if (buffer == NULL) {
		free(result);
		return NULL;
	}

	result->buffer = buffer;
	result->size = length;
	result->cursor = 0;
	result->eof = length == 0;

	return result;
}


bool
ni_stream_eof(NStream* self) {
	return self->eof;
}


uint32_t
ni_stream_length(NStream* self) {
	return self->size;
}


char
ni_stream_peek(NStream* self, bool* end) {
	char result = '\0';
	if (self->cursor == self->size) {
		self->eof = true;
	}
	if (!self->eof) {
		result = self->buffer[self->cursor];
	}
	*end = self->eof;
	return result;
}


char
ni_stream_read(NStream* self, bool* end) {
	char result = ni_stream_peek(self, end);
	if (!self->eof) {
		self->cursor += 1;
	}

	return result;
}
