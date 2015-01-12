#ifndef __N__ASM__READER_H__
#define __N__ASM__READER_H__

typedef struct NInstructionDescriptor NInstructionDescriptor;

#include "lexer.h"
#include "assembler.h"
#include "common/errors.h"

typedef struct {
	uint8_t type;
	int32_t value;
} NInstructionArgument;

struct NInstructionDescriptor {
	uint8_t opcode;
	NInstructionArgument arg_a;
	NInstructionArgument arg_b;
	NInstructionArgument arg_c;
	const char* argument_label;
};


void
ni_read_version(NLexer* lexer,
                uint8_t* major,
                uint8_t* minor,
                uint8_t* revision,
                NError* error);

void
ni_read_entry_point(NLexer* lexer,
                    uint16_t* entry_point,
                    NError* error);

void
ni_read_globals_count(NLexer* lexer,
                      uint16_t* num_globals,
                      NError* error);

void
ni_read_string_constant(NLexer* lexer,
                        char** value,
                        NError* error);

void
ni_read_double_constant(NLexer* lexer,
                        double* value,
                        NError* error);

void
ni_read_character_constant(NLexer* lexer,
                           char** value,
                           NError* error);

void
ni_read_procedure_constant(NLexer* lexer,
                           char** label,
                           uint16_t* num_locals,
                           NError* error);

void
ni_read_int32_constant(NLexer* lexer,
                       int32_t* value,
                       NError* error);

void
ni_read_instruction(NLexer* lexer,
                    NInstructionDescriptor* instruction,
                    NError* error);

void
ni_init_reader();
#endif
