#define SUITE_NAME OStreams

#include "../../test-suite.h"

#include <string.h>

#include "ostreams.h"

#define SINK_FILE_PATH "/dev/null"

TEST(buffers_bigger_than_minimum_have_correct_size) {
	NError error = N_ERROR_INITIALIZER;
	size_t min_plus_100 = N_OSTREAM_MIN_BUFFER_SIZE + 100;
	NOStream* stream = ni_new_memory_ostream(min_plus_100, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_buffer_size(stream), min_plus_100));
}


TEST(buffers_smaller_than_minimum_have_minimum_size) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(N_OSTREAM_MIN_BUFFER_SIZE - 10, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_buffer_size(stream), N_OSTREAM_MIN_BUFFER_SIZE));

}


TEST(newly_created_streams_have_cursor_set_to_zero) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, &error);

	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 0));
}


TEST(writing_uint8_adds_one_to_cursor) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint8(stream, 123, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 1));
}


TEST(writing_uint16_adds_two_to_cursor) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint16(stream, 12345, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 2));
}


TEST(writing_uint32_adds_four_to_cursor) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint32(stream, 123456, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 4));
}


TEST(writing_uint64_adds_eight_to_cursor) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint64(stream, 12345678901, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 8));
}


TEST(file_stream_has_file_pointer) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_file_ostream(SINK_FILE_PATH, 1, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(IS_TRUE(nt_ostream_file(stream) != NULL));
}


TEST(flushing_file_stream_resets_cursor) {
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_file_ostream(SINK_FILE_PATH, 1, &error);
	ASSERT(ERROR_OK(&error));

	ni_ostream_write_uint64(stream, 12345678901, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 8));

	ni_ostream_flush(stream, &error);
	ASSERT(ERROR_OK(&error));
	ASSERT(EQ_SZT(nt_ostream_cursor(stream), 0));

}


TEST(writing_uint8_puts_correct_value_on_buffer) {
	void* buffer;
	uint8_t value;
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint8(stream, 123, &error);
	ASSERT(ERROR_OK(&error));

	buffer = nt_ostream_buffer(stream);
	memcpy((void*) &value, buffer, sizeof(uint8_t));
	ASSERT(EQ_UINT(value, 123));
}

TEST(writing_uint16_puts_correct_value_on_buffer) {
	void* buffer;
	uint16_t value;
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint16(stream, 12345, &error);
	ASSERT(ERROR_OK(&error));

	buffer = nt_ostream_buffer(stream);
	memcpy((void*) &value, buffer, sizeof(uint16_t));
	ASSERT(EQ_UINT(value, 12345));
}

TEST(writing_uint32_puts_correct_value_on_buffer) {
	void* buffer;
	uint32_t value;
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint32(stream, 123, &error);
	ASSERT(ERROR_OK(&error));

	buffer = nt_ostream_buffer(stream);
	memcpy((void*) &value, buffer, sizeof(uint32_t));
	ASSERT(EQ_UINT(value, 123));
}

TEST(writing_uint64_puts_correct_value_on_buffer) {
	void* buffer;
	uint64_t value;
	NError error = N_ERROR_INITIALIZER;
	NOStream* stream =
		ni_new_memory_ostream(1, NULL);

	ni_ostream_write_uint64(stream, 123, &error);
	ASSERT(ERROR_OK(&error));

	buffer = nt_ostream_buffer(stream);
	memcpy((void*) &value, buffer, sizeof(uint64_t));
	ASSERT(EQ_UINT(value, 123));
}
