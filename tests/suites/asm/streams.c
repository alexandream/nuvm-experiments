#define SUITE_NAME Streams

#include <stdio.h>
#include <string.h>

#include "../../test-suite.h"

#include "streams.h"

#define LONG_STREAM_ITERATIONS 50

const char* BASE_PATTERN = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
size_t BASE_LENGTH;

n_stream_t* EMPTY_STREAM;
n_stream_t* SHORT_STREAM;
n_stream_t* LONG_STREAM;

CONSTRUCTOR {
	FILE* file;
	int i;
	file = fopen("empty-file", "w");
	if (file == NULL) SIGNAL_CONSTRUCTOR_ERROR();

	fclose(file);

	file = fopen("short-file", "w");
	if (file == NULL) SIGNAL_CONSTRUCTOR_ERROR();

	if (fputs(BASE_PATTERN, file) < 0) {
		SIGNAL_CONSTRUCTOR_ERROR();
	}
	fclose(file);

	file = fopen("long-file", "w");
	if (file == NULL) SIGNAL_CONSTRUCTOR_ERROR();

	for (i = 0; i < LONG_STREAM_ITERATIONS; i++) {
		if (fputs(BASE_PATTERN, file) < 0) {
			SIGNAL_CONSTRUCTOR_ERROR();
		}
	}
	fclose(file);
	BASE_LENGTH = strlen(BASE_PATTERN);
}


SETUP {
	EMPTY_STREAM = n_new_stream_from_path("empty-file");
	SHORT_STREAM = n_new_stream_from_path("short-file");
	LONG_STREAM  = n_new_stream_from_path("long-file");
}


TEARDOWN {
	n_destroy_stream(EMPTY_STREAM);
	n_destroy_stream(SHORT_STREAM);
	n_destroy_stream(LONG_STREAM);
}


/* Test Cases */

TEST(empty_stream_has_correct_details) {
	ASSERT(n_stream_length(EMPTY_STREAM) == 0);
	ASSERT(n_stream_eof(EMPTY_STREAM) == true);
}


TEST(short_stream_has_correct_details) {
	ASSERT(n_stream_length(SHORT_STREAM) == BASE_LENGTH);
	ASSERT(n_stream_eof(SHORT_STREAM) == false);
}


TEST(long_stream_has_correct_details) {
	ASSERT(n_stream_length(LONG_STREAM) == LONG_STREAM_ITERATIONS * BASE_LENGTH);
	ASSERT(n_stream_eof(LONG_STREAM) == false);
}


TEST(peek_on_empty_stream_signals_eof) {
	bool end = false;
	n_stream_peek(EMPTY_STREAM, &end);
	ASSERT(end);
}


TEST(read_on_empty_stream_signals_eof) {
	bool end = false;
	n_stream_read(EMPTY_STREAM, &end);
	ASSERT(end);
}
