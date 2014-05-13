#define AT_SUITE_NAME Byte_Streams
#include "../../test-suite.h"

#include "common/byte-streams.h"

static uint8_t
input_stream[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                   0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

static int64_t
input_size = (int64_t) sizeof(input_stream);

TEST(reading_initial_byte_works) {
	int64_t i = 0;
	uint8_t result = n_read_byte(input_stream, input_size, &i);

	ASSERT_MSG(result == 0x01, AF("Expected result: 0x01, Got 0x%02X", result));
	ASSERT_MSG(i == 1, AF("Expected new index: 1, Got %d", i));
}


TEST(reading_final_byte_works) {
	int64_t i = input_size -1;
	uint8_t result = n_read_byte(input_stream, input_size, &i);
	ASSERT_MSG(result == 0x0F, AF("Expected result: 0x0F, Got 0x%02X", result));
	ASSERT_MSG(i == input_size, AF("Expected new index: %d, Got %d",
	                                         input_size, i));
}


TEST(reading_middle_byte_works) {
	int64_t i = 5;
	uint8_t result = n_read_byte(input_stream, input_size, &i);
	ASSERT_MSG(result == 0x06, AF("Expected result: 0x06, Got 0x%02X", result));
	ASSERT_MSG(i == 6, AF("Expected new index: 6, Got %d", i));
}


TEST(reading_byte_past_end_fails) {
	int64_t i = input_size;
	n_read_byte(input_stream, input_size, &i);
	ASSERT(i == -1);
}


TEST(reading_initial_int16_works) {
	int64_t i = 0;
	int16_t result = n_read_int16(input_stream, input_size, &i);
	ASSERT(result == 0x0201);
	ASSERT(i = 2);
}


TEST(reading_final_int16_works) {
	int64_t i = input_size -2;
	int16_t result = n_read_int16(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E);
	ASSERT(i = input_size);
}


TEST(reading_middle_int16_works) {
	int64_t i = 6;
	int16_t result = n_read_int16(input_stream, input_size, &i);
	ASSERT(result == 0x0807);
	ASSERT(i = 8);
}


TEST(reading_int16_past_end_fails) {
	int64_t i = input_size;
	n_read_int16(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_int16(input_stream, input_size, &i);
	EXPECT(i == -1);
}


TEST(reading_initial_uint16_works) {
	int64_t i = 0;
	uint16_t result = n_read_uint16(input_stream, input_size, &i);
	ASSERT(result == 0x0201);
	ASSERT(i = 2);
}


TEST(reading_final_uint16_works) {
	int64_t i = input_size -2;
	uint16_t result = n_read_uint16(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E);
	ASSERT(i = input_size);
}


TEST(reading_middle_uint16_works) {
	int64_t i = 3;
	uint16_t result = n_read_uint16(input_stream, input_size, &i);
	ASSERT(result == 0x0504);
	ASSERT(i = 5);
}


TEST(reading_uint16_past_end_fails) {
	int64_t i = input_size;
	n_read_uint16(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_uint16(input_stream, input_size, &i);
	EXPECT(i == -1);
}


TEST(reading_initial_int32_works) {
	int64_t i = 0;
	int32_t result = n_read_int32(input_stream, input_size, &i);
	ASSERT(result == 0x04030201);
	ASSERT(i = 4);
}


TEST(reading_final_int32_works) {
	int64_t i = input_size -4;
	int32_t result = n_read_int32(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E0D0C);
	ASSERT(i = input_size);
}


TEST(reading_middle_int32_works) {
	int64_t i = 6;
	int32_t result = n_read_int32(input_stream, input_size, &i);
	ASSERT(result == 0x0A090807);
	ASSERT(i = 10);
}


TEST(reading_int32_past_end_fails) {
	int64_t i = input_size;
	n_read_int32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_int32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -2;
	n_read_int32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -3;
	n_read_int32(input_stream, input_size, &i);
	EXPECT(i == -1);
}


TEST(reading_initial_uint32_works) {
	int64_t i = 0;
	uint32_t result = n_read_uint32(input_stream, input_size, &i);
	ASSERT(result == 0x04030201);
	ASSERT(i = 4);
}


TEST(reading_final_uint32_works) {
	int64_t i = input_size -4;
	uint32_t result = n_read_uint32(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E0D0C);
	ASSERT(i = input_size);
}


TEST(reading_middle_uint32_works) {
	int64_t i = 6;
	uint32_t result = n_read_uint32(input_stream, input_size, &i);
	ASSERT(result == 0x0A090807);
	ASSERT(i = 10);
}


TEST(reading_uint32_past_end_fails) {
	int64_t i = input_size;
	n_read_uint32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_uint32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -2;
	n_read_uint32(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -3;
	n_read_uint32(input_stream, input_size, &i);
	EXPECT(i == -1);
}


TEST(reading_initial_int64_works) {
	int64_t i = 0;
	int64_t result = n_read_int64(input_stream, input_size, &i);
	ASSERT(result == 0x0807060504030201);
	ASSERT(i = 8);
}


TEST(reading_final_int64_works) {
	int64_t i = input_size -8;
	int64_t result = n_read_int64(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E0D0C0B0A0908);
	ASSERT(i = input_size);
}


TEST(reading_middle_int64_works) {
	int64_t i = 4;
	int64_t result = n_read_int64(input_stream, input_size, &i);
	ASSERT(result == 0x0C0B0A0908070605);
	ASSERT(i = 12);
}


TEST(reading_int64_past_end_fails) {
	int64_t i = input_size;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -2;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -3;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -4;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -5;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -6;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -7;
	n_read_int64(input_stream, input_size, &i);
	EXPECT(i == -1);
}


TEST(reading_initial_uint64_works) {
	int64_t i = 0;
	uint64_t result = n_read_uint64(input_stream, input_size, &i);
	ASSERT(result == 0x0807060504030201);
	ASSERT(i = 8);
}


TEST(reading_final_uint64_works) {
	int64_t i = input_size -8;
	uint64_t result = n_read_uint64(input_stream, input_size, &i);
	ASSERT(result == 0x0F0E0D0C0B0A0908);
	ASSERT(i = input_size);
}


TEST(reading_middle_uint64_works) {
	int64_t i = 4;
	uint64_t result = n_read_uint64(input_stream, input_size, &i);
	ASSERT(result == 0x0C0B0A0908070605);
	ASSERT(i = 12);
}


TEST(reading_uint64_past_end_fails) {
	int64_t i = input_size;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -1;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -2;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -3;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -4;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -5;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -6;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);

	i = input_size -7;
	n_read_uint64(input_stream, input_size, &i);
	EXPECT(i == -1);
}

