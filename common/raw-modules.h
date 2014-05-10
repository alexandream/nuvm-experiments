#ifndef __N__COMMON__RAW_MODULE__H__
#define __N__COMMON__RAW_MODULE__H__

#include "std-types.h"

#define MAGIC_MASK   0x000000FFFFFFFFFF
#define MAGIC_NUMBER 0x0000004D56554E1B

typedef struct n_raw_module_t n_raw_module_t;
typedef struct n_module_descriptor_t n_module_descriptor_t;
typedef struct n_constant_entry_t n_constant_entry_t;


struct n_raw_module_t {
	n_module_descriptor_t *descriptor;

	n_constant_entry_t *constants_index;

	uint8_t *data_segment;
	uint8_t *code_segment;
	uint8_t *extra_segment;
};


struct n_module_descriptor_t {
	uint64_t header;
	uint64_t host_descriptor;

	uint32_t data_size;
	uint32_t code_size;
	uint64_t extra_size;

	uint16_t constants_count;
	uint16_t globals_count;
} __attribute__((packed));


struct n_constant_entry_t {
	uint8_t type_id;
	uint32_t data_index;
} __attribute__((packed));


static bool
n_is_valid_header(uint64_t header) {
	return (header & MAGIC_MASK) == MAGIC_NUMBER;
}


static void
n_read_version(uint64_t header, uint8_t* major, uint8_t* minor, uint8_t* rev) {
	*major = (uint8_t) (header >> 40);
	*minor = (uint8_t) (header >> 48);
	*rev   = (uint8_t) (header >> 56);
}


static uint64_t
n_make_header(uint8_t major, uint8_t minor, uint8_t rev) {
	return MAGIC_NUMBER           |
	       (uint64_t) rev << 56   |
	       (uint64_t) minor << 48 |
	       (uint64_t) major << 40;
}


#undef MAGIC_NUMBER
#undef MAGIC_MASK
#endif
