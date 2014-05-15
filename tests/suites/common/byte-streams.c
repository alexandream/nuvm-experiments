#define AT_SUITE_NAME Byte_Streams
#include "../../test-suite.h"

#include "common/byte-streams.h"

static void
_check_rw_byte_works(int64_t i, int64_t j);

static void
_check_rw_int16_works(int64_t i, int64_t j);

static void
_check_rw_uint16_works(int64_t i, int64_t j);


static uint8_t
ISTREAM[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                   0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

static uint8_t
OSTREAM[sizeof(ISTREAM)];

static int64_t
ISIZE = (int64_t) sizeof(ISTREAM);


TEST(rw_i_i_byte_works) {
	_check_rw_byte_works(0, 0);
}


TEST(rw_i_m_byte_works) {
	_check_rw_byte_works(0, 8);
}


TEST(rw_i_f_byte_works) {
	_check_rw_byte_works(0, ISIZE - sizeof(uint8_t));
}


TEST(rw_f_i_byte_works) {
	_check_rw_byte_works(ISIZE - sizeof(uint8_t), 0);
}


TEST(rw_f_m_byte_works) {
	_check_rw_byte_works(ISIZE - sizeof(uint8_t), 8);
}


TEST(rw_f_f_byte_works) {
	_check_rw_byte_works(ISIZE - sizeof(uint8_t), ISIZE - sizeof(uint8_t));
}


TEST(rw_m_i_byte_works) {
	_check_rw_byte_works(8, 0);
}


TEST(rw_m_m_byte_works) {
	_check_rw_byte_works(8, 8);
}


TEST(rw_m_f_byte_works) {
	_check_rw_byte_works(8, ISIZE - sizeof(uint8_t));
}


TEST(reading_byte_past_end_fails) {
	int64_t i = ISIZE;
	n_read_byte(ISTREAM, ISIZE, &i);
	ASSERT(i == -1);
}


TEST(rw_i_i_int16_works) {
	_check_rw_int16_works(0, 0);
}


TEST(rw_i_m_int16_works) {
	_check_rw_int16_works(0, 7);
}


TEST(rw_i_f_int16_works) {
	_check_rw_int16_works(0, ISIZE - sizeof(int16_t));
}


TEST(rw_f_i_int16_works) {
	_check_rw_int16_works(ISIZE - sizeof(int16_t), 0);
}


TEST(rw_f_m_int16_works) {
	_check_rw_int16_works(ISIZE - sizeof(int16_t), 7);
}


TEST(rw_f_f_int16_works) {
	_check_rw_int16_works(ISIZE - sizeof(int16_t), ISIZE - sizeof(int16_t));
}


TEST(rw_m_i_int16_works) {
	_check_rw_int16_works(7, 0);
}


TEST(rw_m_m_int16_works) {
	_check_rw_int16_works(7, 7);
}


TEST(rw_m_f_int16_works) {
	_check_rw_int16_works(7, ISIZE - sizeof(int16_t));
}


TEST(reading_int16_past_end_fails) {
	int64_t i = ISIZE;
	n_read_int16(ISTREAM, ISIZE, &i);
	EXPECT(i == -1);

	i = ISIZE -1;
	n_read_int16(ISTREAM, ISIZE, &i);
	EXPECT(i == -1);
}


TEST(rw_i_i_uint16_works) {
	_check_rw_uint16_works(0, 0);
}


TEST(rw_i_m_uint16_works) {
	_check_rw_uint16_works(0, 7);
}


TEST(rw_i_f_uint16_works) {
	_check_rw_uint16_works(0, ISIZE - sizeof(uint16_t));
}


TEST(rw_f_i_uint16_works) {
	_check_rw_uint16_works(ISIZE - sizeof(uint16_t), 0);
}


TEST(rw_f_m_uint16_works) {
	_check_rw_uint16_works(ISIZE - sizeof(uint16_t), 7);
}


TEST(rw_f_f_uint16_works) {
	_check_rw_uint16_works(ISIZE - sizeof(uint16_t), ISIZE - sizeof(uint16_t));
}


TEST(rw_m_i_uint16_works) {
	_check_rw_uint16_works(7, 0);
}


TEST(rw_m_m_uint16_works) {
	_check_rw_uint16_works(7, 7);
}


TEST(rw_m_f_uint16_works) {
	_check_rw_uint16_works(7, ISIZE - sizeof(uint16_t));
}


TEST(reading_uint16_past_end_fails) {
	int64_t i = ISIZE;
	n_read_uint16(ISTREAM, ISIZE, &i);
	EXPECT(i == -1);

	i = ISIZE -1;
	n_read_uint16(ISTREAM, ISIZE, &i);
	EXPECT(i == -1);
}



static void
_check_rw_asserts(int64_t i, int64_t j, int64_t i0, int64_t j0,
              uint8_t size, uint8_t psize) {
	int k;
	ASSERT(i == i0 + psize);

	ASSERT(size == psize);
	ASSERT(j == j0 + size);
	for (k = 0; k < size; k++) {
		ASSERT(OSTREAM[j0+k] == ISTREAM[i0+k]);
	}
}

static void
_check_rw_byte_works(int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int16_t result = n_read_byte(ISTREAM, ISIZE, &i);
	uint8_t size = n_write_byte(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint8_t);
	_check_rw_asserts(i, j, initial_i, initial_j, size, psize);
}

static void
_check_rw_int16_works(int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int16_t result = n_read_int16(ISTREAM, ISIZE, &i);
	uint8_t size = n_write_int16(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(int16_t);
	_check_rw_asserts(i, j, initial_i, initial_j, size, psize);
}


static void
_check_rw_uint16_works(int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	uint16_t result = n_read_uint16(ISTREAM, ISIZE, &i);
	uint8_t size = n_write_uint16(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint16_t);
	_check_rw_asserts(i, j, initial_i, initial_j, size, psize);
}
