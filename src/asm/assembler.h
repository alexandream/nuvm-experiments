#ifndef __N__ASM__assembler_H__
#define __N__ASM__assembler_H__
#include <stdio.h>
#include "reader.h"

#include "../common/errors.h"
#include "../common/polyfills/p-stdint.h"

typedef struct NAssembler NAssembler;


typedef enum {
	NI_CONSTANT_STRING = 0,
	NI_CONSTANT_DOUBLE,
	NI_CONSTANT_INT32,
	NI_CONSTANT_PROCEDURE,
	NI_CONSTANT_CHARACTER
} NConstantType;

NAssembler*
ni_new_assembler();

void
ni_destroy_assembler(NAssembler* self);

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
ni_asm_add_instruction(NAssembler* self,
                       NInstruction* instruction,
                       NError* error);

void
ni_asm_add_string_constant(NAssembler* self, char* str);

void
ni_asm_add_double_constant(NAssembler* self, double number);

void
ni_asm_add_character_constant(NAssembler* self, char* utf8_char);

void
ni_asm_add_int32_constant(NAssembler* self, int32_t integer);

void
ni_asm_add_procedure_constant(NAssembler* self,
                              uint16_t label_id,
                              uint16_t nlocals);

uint16_t
ni_asm_get_label(NAssembler* self, const char* label, NError* error);

void
ni_asm_define_label(NAssembler* self, const char* label, NError* error);


void
ni_init_assembler();

void
ni_asm_read_from_lexer(NAssembler* self, NLexer* lexer, NError* error);

size_t
ni_asm_compute_result_size(NAssembler* self, NError* error);

#ifdef TEST_ACCESSORS
uint8_t*
nt_asm_version(NAssembler* self);

uint16_t
nt_asm_entry_point(NAssembler* self);

uint16_t
nt_asm_globals_count(NAssembler* self);

int32_t
nt_asm_constants_count(NAssembler* self);

uint8_t
nt_asm_constant_type(NAssembler* self, int32_t constant);

int64_t
nt_asm_constant_integer(NAssembler* self, int32_t constant);

uint16_t
nt_asm_constant_aux_integer(NAssembler* self, int32_t constant);

int32_t
nt_asm_code_count(NAssembler* self);

NInstruction*
nt_asm_instruction(NAssembler* self, int32_t index);

uint32_t
nt_asm_label_definition(NAssembler* self, int32_t label_id);

#endif /* TEST_ACCESSORS */

#endif /* __N__ASM__assembler_H__ */
