#ifndef __NUVM__UTILS__CONVERTER_H__
#define __NUVM__UTILS__CONVERTER_H__

#include "common.h"

typedef union converter_8_16_t {
	uint16_t whole;
	struct {
		uint8_t lower;
		uint8_t upper;
	};
} converter_8_16_t;

#endif
