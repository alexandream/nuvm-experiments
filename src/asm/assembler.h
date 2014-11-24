#ifndef __N__ASM__builder_H__
#define __N__ASM__builder_H__

#include "../common/polyfills/p-stdint.h"

typedef struct NAssembler NAssembler;

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

/*
void
ni_asm_write_output(NAssembler* self, NStreamWriter* writer, NError* error);
*/

#endif
