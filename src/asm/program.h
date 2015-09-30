#ifndef NHG_A_PROGRAM
#define NHG_A_PROGRAM

#include <stdlib.h>

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

#include "instruction.h"

typedef struct {
	uint8_t type;
	int64_t integer;
	uint16_t aux_integer;
	double real;
	char* text;
} NConstantDescriptor;


typedef struct {
	uint8_t major_version,
	        minor_version,
	        revision;

	uint16_t entry_point;
	uint16_t globals_count;

	NConstantDescriptor* constants;
	size_t constants_size;

	NInstruction* code;
	size_t code_size;

	bool should_free_code;
	bool should_free_constants;
} NProgram;

NProgram*
ni_new_program(NError* error);

void
ni_destroy_program();


#endif /* NHG_A_PROGRAM */
