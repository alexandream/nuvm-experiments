#define SUITE_NAME Streams

#include <stdio.h>
#include <string.h>

#include "../../test-suite.h"

#include "streams.h"

#define LONG_STREAM_ITERATIONS 50

const char* BASE_PATTERN = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
size_t BASE_LENGTH;

ni_stream_t* EMPTY_STREAM;
ni_stream_t* SHORT_STREAM;
ni_stream_t* LONG_STREAM;

/* Write to the used files to guarantee they'll have the expected contents. */
CONSTRUCTOR {
	FILE* file;
	int i;
	file = fopen("build/empty-file", "w");
	if (file == NULL) SIGNAL_CONSTRUCTOR_ERROR();

	fclose(file);

	file = fopen("build/short-file", "w");
	if (file == NULL) SIGNAL_CONSTRUCTOR_ERROR();

	if (fputs(BASE_PATTERN, file) < 0) {
		SIGNAL_CONSTRUCTOR_ERROR();
	}
	fclose(file);

	file = fopen("build/long-file", "w");
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
	EMPTY_STREAM = ni_new_stream_from_path("build/empty-file");
	SHORT_STREAM = ni_new_stream_from_path("build/short-file");
	LONG_STREAM  = ni_new_stream_from_path("build/long-file");
}


TEARDOWN {
	ni_destroy_stream(EMPTY_STREAM);
	ni_destroy_stream(SHORT_STREAM);
	ni_destroy_stream(LONG_STREAM);
}


/* Test Cases */

TEST(empty_stream_has_correct_length) {
	ASSERT(ni_stream_length(EMPTY_STREAM) == 0);
}


TEST(short_stream_has_correct_length) {
	ASSERT(ni_stream_length(SHORT_STREAM) == BASE_LENGTH);
}


TEST(long_stream_has_correct_length) {
	ASSERT(ni_stream_length(LONG_STREAM) == LONG_STREAM_ITERATIONS * BASE_LENGTH);
}


TEST(empty_stream_has_correct_eof) {
	ASSERT(ni_stream_eof(EMPTY_STREAM) == true);
}


TEST(short_stream_has_correct_eof) {
	ASSERT(ni_stream_eof(SHORT_STREAM) == false);
}


TEST(long_stream_has_correct_eof) {
	ASSERT(ni_stream_eof(LONG_STREAM) == false);
}


TEST(peek_on_empty_stream_signals_eof) {
	bool end = false;
	ni_stream_peek(EMPTY_STREAM, &end);
	ASSERT(end == true);
}


TEST(read_on_empty_stream_signals_eof) {
	bool end = false;
	ni_stream_read(EMPTY_STREAM, &end);
	ASSERT(end == true);
}


TEST(first_peek_on_stream_returns_first_entry) {
	bool end = false;
	char entry = ni_stream_peek(SHORT_STREAM, &end);
	ASSERT(end == false);
	ASSERT(entry == 'A');
}


TEST(first_read_on_stream_returns_first_entry) {
	bool end = false;
	char entry = ni_stream_read(SHORT_STREAM, &end);
	ASSERT(end == false);
	ASSERT(entry == 'A');
}


TEST(second_peek_on_stream_returns_first_entry) {
	bool end = false;
	char first_entry = ni_stream_peek(SHORT_STREAM, &end);
	char entry = ni_stream_peek(SHORT_STREAM, &end);
	ASSERT(end == false);
	ASSERT(entry == first_entry);
	ASSERT(entry == 'A');
}


TEST(second_read_on_stream_returns_second_entry) {
	bool end = false;
	char first_entry = ni_stream_read(SHORT_STREAM, &end);
	char entry = ni_stream_read(SHORT_STREAM, &end);
	ASSERT(end == false);
	ASSERT(entry != first_entry);
	ASSERT(entry == 'B');
}
