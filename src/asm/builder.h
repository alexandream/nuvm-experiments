#ifndef __N__ASM__builder_H__
#define __N__ASM__builder_H__

#include "lexer.h"
#include "common/errors.h"

struct NStreamWriter {
	NStreamWriter_vtable* vtable;
};

struct NStreamWriter_vtable {
	void (*write_bytes)(NStreamWriter* self,
	                    uint8_t* data,
	                    uint32_t size,
	                    NError* error);

	void (*close)(NStreamWriter* self, NError* error);
}


typedef struct NAssembler NAssembler;
typedef struct NConstantDescriptor NConstantDescriptor;
typedef struct NPointerArray NPointerArray;

struct NConstantDescriptor {
	uint8_t type;
	uint64_t integer;
	double real;
	char* text;
}

struct NPointerArray {
	int32_t capacity;
	int32_t size;
	void** values;
}:


struct NAssembler {
	uint8_t version[3];
	uint16_t entry_point;
	uint16_t globals_count;

	NPointerArray constants; /* <NConstantDescriptor> */

	Map<String, Int> 
}


void
ni_asm_set_version(NAssembler* self,
                   uint8_t major,
                   uint8_t minor,
                   uint8_t rev);

void
ni_asm_set_entry_point(NAssembler* self, uint16_t entry_point);

void
ni_asm_set_globals_count(NAssembler* self, uint16_t globals_count);

void
ni_asm_add_string_constant(NAssembler* self, const char* str);

void
ni_asm_add_double_constant(NAssembler* self, double number);

void
ni_asm_add_character_constant(NAssembler* self, const char* utf8_char);

void
ni_asm_add_int32_constant(NAssembler* self, int32_t integer);

void
ni_asm_add_procedure_constant(NAssembler* self,
                              uint16_t label_id,
                              uint16_t nlocals);

uint16_t
ni_asm_get_label(NAssembler* self, const char* label);

void
ni_asm_write_output(NAssembler* self, NStreamWriter* writer, NError* error);

#endif
