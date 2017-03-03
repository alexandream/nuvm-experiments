#define SUITE_NAME Byte_Streams
#include "../../test-suite.h"

#include "common/byte-streams.h"

static void
_check_byte(ATResult* r, int64_t i, int64_t j);

static void
_check_int16(ATResult* r, int64_t i, int64_t j);

static void
_check_uint16(ATResult* r, int64_t i, int64_t j);

static void
_check_int32(ATResult* r, int64_t i, int64_t j);

static void
_check_uint32(ATResult* r, int64_t i, int64_t j);

static void
_check_int64(ATResult* r, int64_t i, int64_t j);

static void
_check_uint64(ATResult* r, int64_t i, int64_t j);

#define ASSERT_RW_BYTE(i, j)   _check_byte(ni_g_test_result, i, j)
#define ASSERT_RW_INT16(i, j)  _check_int16(ni_g_test_result, i, j)
#define ASSERT_RW_INT32(i, j)  _check_int32(ni_g_test_result, i, j)
#define ASSERT_RW_INT64(i, j)  _check_int64(ni_g_test_result, i, j)
#define ASSERT_RW_UINT16(i, j) _check_uint16(ni_g_test_result, i, j)
#define ASSERT_RW_UINT32(i, j) _check_uint32(ni_g_test_result, i, j)
#define ASSERT_RW_UINT64(i, j) _check_uint64(ni_g_test_result, i, j)

static uint8_t
ISTREAM[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                   0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

static uint8_t
OSTREAM[sizeof(ISTREAM)];

static int64_t
ISIZE = (int64_t) sizeof(ISTREAM);


TEST(rw_i_i_byte_works) {
	ASSERT_RW_BYTE(0, 0);
}


TEST(rw_i_m_byte_works) {
	ASSERT_RW_BYTE(0, 8);
}


TEST(rw_i_f_byte_works) {
	ASSERT_RW_BYTE(0, ISIZE - sizeof(uint8_t));
}


TEST(rw_f_i_byte_works) {
	ASSERT_RW_BYTE(ISIZE - sizeof(uint8_t), 0);
}


TEST(rw_f_m_byte_works) {
	ASSERT_RW_BYTE(ISIZE - sizeof(uint8_t), 8);
}


TEST(rw_f_f_byte_works) {
	ASSERT_RW_BYTE(ISIZE - sizeof(uint8_t), ISIZE - sizeof(uint8_t));
}


TEST(rw_m_i_byte_works) {
	ASSERT_RW_BYTE(8, 0);
}


TEST(rw_m_m_byte_works) {
	ASSERT_RW_BYTE(8, 8);
}


TEST(rw_m_f_byte_works) {
	ASSERT_RW_BYTE(8, ISIZE - sizeof(uint8_t));
}


TEST(rw_i_i_int16_works) {
	ASSERT_RW_INT16(0, 0);
}


TEST(rw_i_m_int16_works) {
	ASSERT_RW_INT16(0, 7);
}


TEST(rw_i_f_int16_works) {
	ASSERT_RW_INT16(0, ISIZE - sizeof(int16_t));
}


TEST(rw_f_i_int16_works) {
	ASSERT_RW_INT16(ISIZE - sizeof(int16_t), 0);
}


TEST(rw_f_m_int16_works) {
	ASSERT_RW_INT16(ISIZE - sizeof(int16_t), 7);
}


TEST(rw_f_f_int16_works) {
	ASSERT_RW_INT16(ISIZE - sizeof(int16_t), ISIZE - sizeof(int16_t));
}


TEST(rw_m_i_int16_works) {
	ASSERT_RW_INT16(7, 0);
}


TEST(rw_m_m_int16_works) {
	ASSERT_RW_INT16(7, 7);
}


TEST(rw_m_f_int16_works) {
	ASSERT_RW_INT16(7, ISIZE - sizeof(int16_t));
}


TEST(rw_i_i_uint16_works) {
	ASSERT_RW_UINT16(0, 0);
}


TEST(rw_i_m_uint16_works) {
	ASSERT_RW_UINT16(0, 7);
}


TEST(rw_i_f_uint16_works) {
	ASSERT_RW_UINT16(0, ISIZE - sizeof(uint16_t));
}


TEST(rw_f_i_uint16_works) {
	ASSERT_RW_UINT16(ISIZE - sizeof(uint16_t), 0);
}


TEST(rw_f_m_uint16_works) {
	ASSERT_RW_UINT16(ISIZE - sizeof(uint16_t), 7);
}


TEST(rw_f_f_uint16_works) {
	ASSERT_RW_UINT16(ISIZE - sizeof(uint16_t), ISIZE - sizeof(uint16_t));
}


TEST(rw_m_i_uint16_works) {
	ASSERT_RW_UINT16(7, 0);
}


TEST(rw_m_m_uint16_works) {
	ASSERT_RW_UINT16(7, 7);
}


TEST(rw_m_f_uint16_works) {
	ASSERT_RW_UINT16(7, ISIZE - sizeof(uint16_t));
}


TEST(rw_i_i_int32_works) {
	ASSERT_RW_INT32(0, 0);
}


TEST(rw_i_m_int32_works) {
	ASSERT_RW_INT32(0, 6);
}


TEST(rw_i_f_int32_works) {
	ASSERT_RW_INT32(0, ISIZE - sizeof(int32_t));
}


TEST(rw_m_i_int32_works) {
	ASSERT_RW_INT32(6, 0);
}


TEST(rw_m_m_int32_works) {
	ASSERT_RW_INT32(6, 6);
}


TEST(rw_m_f_int32_works) {
	ASSERT_RW_INT32(6, ISIZE - sizeof(int32_t));
}


TEST(rw_f_i_int32_works) {
	ASSERT_RW_INT32(ISIZE - sizeof(int32_t), 0);
}


TEST(rw_f_m_int32_works) {
	ASSERT_RW_INT32(ISIZE - sizeof(int32_t), 6);
}


TEST(rw_f_f_int32_works) {
	ASSERT_RW_INT32(ISIZE - sizeof(int32_t), ISIZE - sizeof(int32_t));
}


TEST(rw_i_i_uint32_works) {
	ASSERT_RW_UINT32(0, 0);
}


TEST(rw_i_m_uint32_works) {
	ASSERT_RW_UINT32(0, 6);
}


TEST(rw_i_f_uint32_works) {
	ASSERT_RW_UINT32(0, ISIZE - sizeof(uint32_t));
}


TEST(rw_m_i_uint32_works) {
	ASSERT_RW_UINT32(6, 0);
}


TEST(rw_m_m_uint32_works) {
	ASSERT_RW_UINT32(6, 6);
}


TEST(rw_m_f_uint32_works) {
	ASSERT_RW_UINT32(6, ISIZE - sizeof(uint32_t));
}


TEST(rw_f_i_uint32_works) {
	ASSERT_RW_UINT32(ISIZE - sizeof(uint32_t), 0);
}


TEST(rw_f_m_uint32_works) {
	ASSERT_RW_UINT32(ISIZE - sizeof(uint32_t), 6);
}


TEST(rw_f_f_uint32_works) {
	ASSERT_RW_UINT32(ISIZE - sizeof(uint32_t), ISIZE - sizeof(uint32_t));
}


TEST(rw_i_i_int64_works) {
	ASSERT_RW_INT64(0, 0);
}


TEST(rw_i_m_int64_works) {
	ASSERT_RW_INT64(0, 6);
}


TEST(rw_i_f_int64_works) {
	ASSERT_RW_INT64(0, ISIZE - sizeof(int64_t));
}


TEST(rw_m_i_int64_works) {
	ASSERT_RW_INT64(6, 0);
}


TEST(rw_m_m_int64_works) {
	ASSERT_RW_INT64(6, 6);
}


TEST(rw_m_f_int64_works) {
	ASSERT_RW_INT64(6, ISIZE - sizeof(int64_t));
}


TEST(rw_f_i_int64_works) {
	ASSERT_RW_INT64(ISIZE - sizeof(int64_t), 0);
}


TEST(rw_f_m_int64_works) {
	ASSERT_RW_INT64(ISIZE - sizeof(int64_t), 6);
}


TEST(rw_f_f_int64_works) {
	ASSERT_RW_INT64(ISIZE - sizeof(int64_t), ISIZE - sizeof(int64_t));
}


TEST(rw_i_i_uint64_works) {
	ASSERT_RW_UINT64(0, 0);
}


TEST(rw_i_m_uint64_works) {
	ASSERT_RW_UINT64(0, 6);
}


TEST(rw_i_f_uint64_works) {
	ASSERT_RW_UINT64(0, ISIZE - sizeof(uint64_t));
}


TEST(rw_m_i_uint64_works) {
	ASSERT_RW_UINT64(6, 0);
}


TEST(rw_m_m_uint64_works) {
	ASSERT_RW_UINT64(6, 6);
}


TEST(rw_m_f_uint64_works) {
	ASSERT_RW_UINT64(6, ISIZE - sizeof(uint64_t));
}


TEST(rw_f_i_uint64_works) {
	ASSERT_RW_UINT64(ISIZE - sizeof(uint64_t), 0);
}


TEST(rw_f_m_uint64_works) {
	ASSERT_RW_UINT64(ISIZE - sizeof(uint64_t), 6);
}


TEST(rw_f_f_uint64_works) {
	ASSERT_RW_UINT64(ISIZE - sizeof(uint64_t), ISIZE - sizeof(uint64_t));
}


static void
_check_rw_asserts(ATResult* ni_g_test_result, int64_t i, int64_t j, int64_t i0, int64_t j0,
              uint8_t size, uint8_t psize) {
	int k;
	ASSERT(EQ_INT(i, i0 + psize));

	ASSERT(EQ_INT(size, psize));
	ASSERT(EQ_INT(j, j0 + size));
	for (k = 0; k < size; k++) {
		ASSERT(EQ_INT(OSTREAM[j0+k],ISTREAM[i0+k]));
	}
}

static void
_check_byte(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int16_t result = ni_read_byte(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_byte(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint8_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}

static void
_check_int16(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int16_t result = ni_read_int16(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_int16(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(int16_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}


static void
_check_uint16(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	uint16_t result = ni_read_uint16(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_uint16(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint16_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}


static void
_check_int32(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int32_t result = ni_read_int32(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_int32(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(int32_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}


static void
_check_uint32(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	uint32_t result = ni_read_uint32(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_uint32(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint32_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}


static void
_check_int64(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	int64_t result = ni_read_int64(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_int64(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(int64_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}


static void
_check_uint64(ATResult* r, int64_t i, int64_t j) {
	int64_t initial_i = i;
	int64_t initial_j = j;
	uint64_t result = ni_read_int64(ISTREAM, ISIZE, &i);
	uint8_t size = ni_write_int64(OSTREAM, sizeof(OSTREAM), &j, result);

	uint8_t psize = sizeof(uint64_t);
	_check_rw_asserts(r, i, j, initial_i, initial_j, size, psize);
}
