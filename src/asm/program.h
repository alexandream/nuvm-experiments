#ifndef NHG_A_PROGRAM
#define NHG_A_PROGRAM

#include <stdlib.h>

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

#include "instruction.h"

#define NI_CONSTANT_INITIALIZER { 0xFF, 0, 0, 0.0, NULL }

enum NConstantType {
	NI_CONSTANT_STRING = 0,
	NI_CONSTANT_DOUBLE,
	NI_CONSTANT_INT32,
	NI_CONSTANT_PROCEDURE,
	NI_CONSTANT_CHARACTER
};

typedef struct {
	uint8_t type;
	int32_t int32;
	uint16_t uint16;
	double real;
	char* text;
	size_t text_length;
	uint16_t label_id;
	uint32_t label_definition;
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
