#ifndef __NUVM__UTIL__CONVERTER_H__
#define __NUVM__UTIL__CONVERTER_H__

#include "types.h"

static uint16_t
pack_8s_as_16(uint8_t upper, uint8_t lower) {
	return (upper << 8) | lower;
}

static void
unpack_16_to_8s(uint16_t whole, uint8_t* upper, uint8_t *lower) {
	*upper = whole >> 8;
	*lower = 0x00FF & whole;
}
#endif

