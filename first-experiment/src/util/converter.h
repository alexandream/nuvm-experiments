#ifndef __NUVM__UTILS__CONVERTER_H__
#define __NUVM__UTILS__CONVERTER_H__

#include "common.h"

// FIXME [0001]: Assumed endianess
typedef union converter_8_16_t {
	uint16_t whole;
	struct {
		uint8_t lower;
		uint8_t upper;
	};
} converter_8_16_t;

static inline
uint16_t pack_8s_to_16(uint8_t upper, uint8_t lower) {
	converter_8_16_t packer;
	packer.upper = upper;
	packer.lower = lower;
	return packer.whole;
}

static inline
void unpack_16_to_8s(uint16_t whole, uint8_t *upper, uint8_t *lower) {
	converter_8_16_t packed;
	packed.whole = whole;
	*upper = packed.upper;
	*lower = packed.lower;
}


#endif
