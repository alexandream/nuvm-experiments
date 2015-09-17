#define SUITE_NAME IStreams

#include <stdio.h>
#include <string.h>

#include "../../test-suite.h"

#include "istreams.h"

#define LONG_STREAM_ITERATIONS 50

const char* BASE_PATTERN = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
size_t BASE_LENGTH;

NIStream* EMPTY_STREAM;
NIStream* SHORT_STREAM;
NIStream* LONG_STREAM;

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
	EMPTY_STREAM = ni_new_istream_from_path("build/empty-file", NULL);
	SHORT_STREAM = ni_new_istream_from_path("build/short-file", NULL);
	LONG_STREAM  = ni_new_istream_from_path("build/long-file", NULL);
}


TEARDOWN {
	ni_destroy_istream(EMPTY_STREAM);
	ni_destroy_istream(SHORT_STREAM);
	ni_destroy_istream(LONG_STREAM);
}


/* Test Cases */

TEST(empty_istream_has_correct_eof) {
	ASSERT(IS_TRUE(ni_istream_eof(EMPTY_STREAM) == true));
}


TEST(short_istream_has_correct_eof) {
	ASSERT(IS_TRUE(ni_istream_eof(SHORT_STREAM) == false));
}

TEST(short_istream_has_correct_length) {
	int i = 0;
	while(!ni_istream_eof(SHORT_STREAM)) {
		i++;
		ni_istream_read(SHORT_STREAM, NULL);

	}
	ASSERT(EQ_SZT(i, BASE_LENGTH));
}



TEST(long_istream_has_correct_eof) {
	ASSERT(IS_TRUE(ni_istream_eof(LONG_STREAM) == false));
}


TEST(long_istream_has_correct_length) {
	int i = 0;
	while(!ni_istream_eof(LONG_STREAM)) {
		i++;
		ni_istream_read(LONG_STREAM, NULL);

	}
	ASSERT(EQ_SZT(i, BASE_LENGTH * LONG_STREAM_ITERATIONS));
}



TEST(peek_on_empty_istream_signals_eof) {
	bool end = false;
	ni_istream_peek(EMPTY_STREAM, &end);
	ASSERT(IS_TRUE(end == true));
}


TEST(read_on_empty_istream_signals_eof) {
	bool end = false;
	ni_istream_read(EMPTY_STREAM, &end);
	ASSERT(IS_TRUE(end == true));
}


TEST(first_peek_on_istream_returns_first_entry) {
	bool end = false;
	char entry = ni_istream_peek(SHORT_STREAM, &end);
	ASSERT(IS_TRUE(end == false));
	ASSERT(IS_TRUE(entry == 'A'));
}


TEST(first_read_on_istream_returns_first_entry) {
	bool end = false;
	char entry = ni_istream_read(SHORT_STREAM, &end);
	ASSERT(IS_TRUE(end == false));
	ASSERT(IS_TRUE(entry == 'A'));
}


TEST(second_peek_on_istream_returns_first_entry) {
	bool end = false;
	char first_entry = ni_istream_peek(SHORT_STREAM, &end);
	char entry = ni_istream_peek(SHORT_STREAM, &end);
	ASSERT(IS_TRUE(end == false));
	ASSERT(IS_TRUE(entry == first_entry));
	ASSERT(IS_TRUE(entry == 'A'));
}


TEST(second_read_on_istream_returns_second_entry) {
	bool end = false;
	char first_entry = ni_istream_read(SHORT_STREAM, &end);
	char entry = ni_istream_read(SHORT_STREAM, &end);
	ASSERT(IS_TRUE(end == false));
	ASSERT(IS_TRUE(entry != first_entry));
	ASSERT(IS_TRUE(entry == 'B'));
}
