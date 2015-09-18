#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/polyfills/p-strdup.h"

#include "errors.h"
#include "istreams.h"

struct NIStream {
	char* buffer;
	int32_t size;
	int32_t cursor;
};

void
ni_destroy_istream(NIStream* self) {
	free(self);
}


NIStream*
ni_new_istream_from_path(const char* path, NError *error) {
	/* TODO : This should not load the file into memory. */
	/*   This function is very very stupid. It creates a "stream" by loading
	 * the whole file into memory and acting as if it's a big string.
	 *   For now this behaviour is good enough, but it should be turned into
	 * a real stream and do buffered input from the file. */
	int status;
	int32_t file_size, i;
	char* buffer = NULL;
	uint32_t error_type = 0;
	NIStream* result = NULL;

	FILE* file = NULL;

	n_error_reset(error);

	file = fopen(path, "rb");
	if (file == NULL) {
		error_type = ni_a_errors.IOError;
		goto cleanup;
	}

	status = fseek(file, 0, SEEK_END);
	if (status != 0) {
		error_type = ni_a_errors.IOError;
		goto cleanup;
	}

	file_size = ftell(file);
	if (file_size < 0) {
		error_type = ni_a_errors.IOError;
		goto cleanup;
	}

	status = fseek(file, 0, SEEK_SET);
	if (status != 0) {
		error_type = ni_a_errors.IOError;
		goto cleanup;
	}

	result = malloc(sizeof(NIStream));
	if (result == NULL) {
		error_type = ni_a_errors.BadAllocation;
		goto cleanup;
	}

	if (file_size > 0) {
		buffer = malloc(sizeof(char) * (file_size + 1));
		if (buffer == NULL) {
			error_type = ni_a_errors.BadAllocation;
			goto cleanup;
		}

		i = 0;
		while (!feof(file)) {
			size_t n = fread(buffer + i, sizeof(char), file_size - i + 1, file);
			if (ferror(file)) {
				error_type = ni_a_errors.IOError;
				goto cleanup;
			}
			i += n;
		}
		buffer[file_size] = '\0';
	}
	result->buffer = buffer;
	result->size = file_size;
	result->cursor = 0;
	fclose(file);
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
	if (error != NULL) {
		/* TODO: Error should contain more information about the situation */
		error->type = error_type;
	}
	return NULL;
}


NIStream*
ni_new_istream_from_string(const char* string) {
	/* TODO: Add error reporting and detailed error checking. */
	size_t length = strlen(string);
	NIStream* result = NULL;
	char* buffer = NULL;
	if (length > INT32_MAX) {
		return NULL;
	}

	result = malloc(sizeof(NIStream));
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

	return result;
}


bool
ni_istream_eof(NIStream* self) {
	return self->cursor == self->size;
}


char
ni_istream_peek(NIStream* self, bool* end) {
	char result = '\0';
	bool finished = true;
	if (!ni_istream_eof(self)) {
		result = self->buffer[self->cursor];
		finished = false;
	}
	if (end != NULL) {
		*end = finished;
	}
	return result;
}


char
ni_istream_read(NIStream* self, bool* end) {
	bool finished;
	char result = ni_istream_peek(self, &finished);
	if (!finished) {
		self->cursor += 1;
	}
	if (end != NULL) {
		*end = finished;
	}
	return result;
}
