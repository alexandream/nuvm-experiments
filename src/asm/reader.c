#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "errors.h"
#include "reader.h"
#include "opcodes.h"

#include "common/polyfills/p-strdup.h"

#define CREG NI_RT_CONSTANT
#define GREG NI_RT_GLOBAL
#define LREG NI_RT_LOCAL

#define INT24_MIN  -8388608
#define INT24_MAX   8388607
#define UINT22_MAX  4194303

static void
expect_token_type(NLexer* lexer, NTokenType expected_type, NError* error);

static void
consume_dec_int(NLexer* lexer, int32_t* value, NError* error);

static void
consume_dec_int_u16(NLexer* lexer, uint16_t* value, NError* error);

static void
consume_dec_int_u8(NLexer* lexer, uint8_t* value, NError* error);

static void
consume_double(NLexer* lexer, double* value, NError* error);

static void
consume_label(NLexer* lexer, char** value, NError* error);

static uint8_t
consume_opcode(NLexer* lexer, NError* error);

static void
consume_string(NLexer* lexer, char** value, size_t* length, NError* error);

static int32_t
parse_dec_integer(const char* lexeme);

static int32_t
parse_register(NLexer* lexer, uint8_t bits, NError* error);

static void
read_register_based_instruction(NLexer* lexer,
                                NInstruction* instruction,
                                uint8_t reg_a_bits,
                                uint8_t reg_b_bits,
                                uint8_t reg_c_bits,
                                NError* error);

static void
ni_read_arith_rel_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error);


static void
ni_read_global_ref_instruction(NLexer* lexer,
                               NInstruction* instruction,
                               NError* error);

static void
ni_read_global_set_instruction(NLexer* lexer,
                               NInstruction* instruction,
                               NError* error);

static void
ni_read_const_ref_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error);

static void
ni_read_jump_instruction(NLexer* lexer,
                         NInstruction* instruction,
                         NError* error);

static void
ni_read_jump_if_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error);

static void
ni_read_load_bool_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error);

static void
ni_read_logical_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error);

static void
ni_read_move_in_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error);

static void
ni_read_move_out_instruction(NLexer* lexer,
                             NInstruction* instruction,
                             NError* error);

static void
ni_read_not_instruction(NLexer* lexer,
                        NInstruction* instruction,
                        NError* error);

static void
ni_read_return_instruction(NLexer* lexer,
                           NInstruction* instruction,
                           NError* error);

void
ni_read_version(NLexer* lexer,
                uint8_t* major,
                uint8_t* minor,
                uint8_t* revision,
                NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_VERSION, error);
	if (!n_error_ok(error)) {
		return;
	}

	ni_lexer_advance(lexer);

	consume_dec_int_u8(lexer, major, error);
	if (!n_error_ok(error)) {
		return;
	}

	consume_dec_int_u8(lexer, minor, error);
	if (!n_error_ok(error)) {
		return;
	}

	consume_dec_int_u8(lexer, revision, error);
}


void
ni_read_entry_point(NLexer* lexer,
                    uint16_t* entry_point,
                    NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_ENTRY_POINT, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_dec_int_u16(lexer, entry_point, error);
}


void
ni_read_globals_count(NLexer* lexer,
                      uint16_t* num_globals,
                      NError* error) {
	expect_token_type(lexer, NI_TK_KW_GLOBALS_COUNT, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_dec_int_u16(lexer, num_globals, error);
}


void
ni_read_string_constant(NLexer* lexer,
                        char** value,
						size_t* length,
                        NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_STRING, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_string(lexer, value, length, error);
}


void
ni_read_double_constant(NLexer* lexer,
                        double* value,
                        NError* error) {
	expect_token_type(lexer, NI_TK_KW_DOUBLE, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_double(lexer, value, error);
}


void
ni_read_character_constant(NLexer* lexer,
                           char** value,
						   size_t* length,
                           NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_CHARACTER, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_string(lexer, value, length, error);
}


void
ni_read_procedure_constant(NLexer* lexer,
                           char** label,
                           uint16_t* num_locals,
                           NError* error) {
	expect_token_type(lexer, NI_TK_KW_PROCEDURE, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_label(lexer, label, error);
	if (!n_error_ok(error)) return;

	consume_dec_int_u16(lexer, num_locals, error);
}


void
ni_read_int32_constant(NLexer* lexer,
                       int32_t* value,
                       NError* error) {
	expect_token_type(lexer, NI_TK_KW_INT32, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_dec_int(lexer, value, error);
}


void
ni_read_instruction(NLexer* lexer,
                    NInstruction* instruction,
                    NError* error) {
	NTokenType opcode_token = ni_lexer_peek(lexer);
	switch(opcode_token) {
		case NI_TK_OP_EQ:
		case NI_TK_OP_GE:
		case NI_TK_OP_GT:
		case NI_TK_OP_LE:
		case NI_TK_OP_LT:
		case NI_TK_OP_ADD:
		case NI_TK_OP_DIV:
		case NI_TK_OP_MUL:
		case NI_TK_OP_SUB:
			ni_read_arith_rel_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_AND:
		case NI_TK_OP_OR:
			ni_read_logical_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_MOVE_IN:
			ni_read_move_in_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_MOVE_OUT:
			ni_read_move_out_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_NOT:
			ni_read_not_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_GLOBAL_REF:
			ni_read_global_ref_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_GLOBAL_SET:
			ni_read_global_set_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_CONST_REF:
			ni_read_const_ref_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_LOAD_BOOL:
			ni_read_load_bool_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_JUMP:
			ni_read_jump_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_JUMP_IF:
			ni_read_jump_if_instruction(lexer, instruction, error);
			return;
		case NI_TK_OP_RETURN:
			ni_read_return_instruction(lexer, instruction, error);
			return;
		default:
			n_error_set(error, ni_a_errors.reader.UnimplementedOpcode, NULL);
			return;
	}
}

static void
ni_read_arith_rel_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 8, 8, error);
}


static void
ni_read_move_in_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 16, 0, error);
}


static void
ni_read_move_out_instruction(NLexer* lexer,
                             NInstruction* instruction,
                             NError* error) {
	read_register_based_instruction(lexer, instruction, 16, 8, 0, error);
}


static void
ni_read_global_ref_instruction(NLexer* lexer,
                               NInstruction* instruction,
                               NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 16, 0, error);
}


static void
ni_read_const_ref_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 16, 0, error);
}


static void
ni_read_global_set_instruction(NLexer* lexer,
                               NInstruction* instruction,
                               NError* error) {
	read_register_based_instruction(lexer, instruction, 16, 8, 0, error);
}


static void
ni_read_jump_instruction(NLexer* lexer,
                         NInstruction* instruction,
                         NError* error) {
	char* label_name = NULL;
	uint8_t opcode = consume_opcode(lexer, error);

	if (!n_error_ok(error)) return;

	instruction->opcode = opcode;

	consume_label(lexer, &label_name, error);
	if (!n_error_ok(error)) return;

	instruction->argument_label = label_name;
}


static void
ni_read_jump_if_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error) {
	char* label_name = NULL;
	uint8_t opcode = consume_opcode(lexer, error);
	if (!n_error_ok(error)) return;


	instruction->opcode = opcode;

	instruction->arg_a = parse_register(lexer, 8, error);
	if (!n_error_ok(error)) return;


	consume_label(lexer, &label_name, error);
	if (!n_error_ok(error)) return;

	instruction->argument_label = label_name;
}


static void
ni_read_load_bool_instruction(NLexer* lexer,
                              NInstruction* instruction,
                              NError* error) {
	int32_t value = 0;
	uint8_t opcode = consume_opcode(lexer, error);
	if (!n_error_ok(error)) return;

	instruction->opcode = opcode;

	instruction->arg_a = parse_register(lexer, 8, error);
	if (!n_error_ok(error)) return;

	consume_dec_int(lexer, &value, error);
	if (!n_error_ok(error)) return;

	if (value < 0 || value > 1) {
		n_error_set(error, ni_a_errors.reader.RegisterOutOfRange, NULL);
		return;
	}
	instruction->arg_b = value;
}


static void
ni_read_logical_instruction(NLexer* lexer,
                            NInstruction* instruction,
                            NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 8, 8, error);
}


static void
ni_read_not_instruction(NLexer* lexer,
                        NInstruction* instruction,
                        NError* error) {
	read_register_based_instruction(lexer, instruction, 8, 16, 0, error);
}


static void
ni_read_return_instruction(NLexer* lexer,
                           NInstruction* instruction,
                           NError* error) {
	read_register_based_instruction(lexer, instruction, 16, 0, 0, error);
}


static void
read_register_based_instruction(NLexer* lexer,
                                NInstruction* instruction,
                                uint8_t reg_a_bits,
                                uint8_t reg_b_bits,
                                uint8_t reg_c_bits,
                                NError* error) {
	uint8_t opcode = consume_opcode(lexer, error);
	if (!n_error_ok(error)) return;
	instruction->opcode = opcode;

	if (reg_a_bits > 0) {
		instruction->arg_a = parse_register(lexer, reg_a_bits, error);
		if (!n_error_ok(error)) return;
	}

	if (reg_b_bits > 0) {
		instruction->arg_b = parse_register(lexer, reg_b_bits, error);
		if (!n_error_ok(error)) return;
	}

	if (reg_c_bits > 0) {
		instruction->arg_c = parse_register(lexer, reg_c_bits, error);
		if (!n_error_ok(error)) return;
	}
}




static int32_t
parse_register(NLexer* lexer, uint8_t bits, NError* error) {
	int32_t max_value, value;

	switch(bits) {
		case 8:
			max_value = UINT8_MAX;
			break;
		case 16:
			max_value = UINT16_MAX;
			break;
		default:
			/* This shouldn't happen. Just a guard here. */
			exit(-1);
	}

	consume_dec_int(lexer, &value, error);
	if (!n_error_ok(error)) return INT32_MIN;

	if (value < 0 || value > max_value) {
		/* TODO: Put more information in this error result. */
		/* What value did we find? What range is ok? Which argument? */
		n_error_set(error, ni_a_errors.reader.RegisterOutOfRange, NULL);
		return INT32_MIN;
	}
	return value;
}



static void
expect_token_type(NLexer* lexer, NTokenType expected_type, NError* error) {
	NTokenType cur_token_type = ni_lexer_peek(lexer);
	n_error_reset(error);

	if (cur_token_type != expected_type) {
		/* TODO (#1): Generate proper error. */
		if (cur_token_type == NI_TK_EOF) {
			n_error_set(error, ni_a_errors.reader.EndOfFile, NULL);
		}
		else {
			n_error_set(error, ni_a_errors.reader.UnexpectedToken, NULL);
		}
		return;
	}
	return;
}

static int32_t
parse_dec_integer(const char* lexeme) {
	char* end_ptr;
	int32_t result = strtol(lexeme, &end_ptr, 10);
	assert(*end_ptr == '\0');
	assert(errno != ERANGE);
	return result;
}

#include <stdio.h>
static char*
parse_string(NToken token, size_t* len) {
	char* result;
	char* input = token.lexeme;
	size_t length = strlen(input);
	size_t i = 0, j = 0;

	/* Input string should be at least of size 2, for it must begin and
	 * end with quote characters. Must also be ended by a nul character. */
	assert(length >= 2);
	assert(input[0] == '"');
	assert(input[length] == '\0');
	assert(input[length-1] == '"');

	/* TODO (#3): Check if we should use a different allocator here. */
	result = malloc(sizeof(char) * length+1);
	assert(result != NULL);
	for (i = 1; i < length - 1; i++) {
		if (input[i] == '\\') {
			/* TODO (#2): Implement proper escaping, right now we only handle
			 * the escaped quote (\") case correctly. */
			i++;
		}
		result[j] = input[i];
		j++;
	}
	result[j] = '\0';
	*len = j;
	return result;
}


static void
consume_dec_int(NLexer* lexer, int32_t* value, NError* error) {
	NToken cur_token;
	expect_token_type(lexer, NI_TK_DEC_INTEGER, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = parse_dec_integer(cur_token.lexeme);
	ni_destruct_token(&cur_token);
}


static void
consume_dec_int_u16(NLexer* lexer, uint16_t* value, NError* error) {
	int32_t value_i32 = 0;
	consume_dec_int(lexer, &value_i32, error);
	if (value_i32 < 0 || value_i32 > UINT16_MAX) {
		n_error_set(error, ni_a_errors.NumberOutOfBounds, NULL);
		return;
	}
	*value = value_i32;
	return;
}


static void
consume_dec_int_u8(NLexer* lexer, uint8_t* value, NError* error) {
	int32_t value_i32 = 0;
	consume_dec_int(lexer, &value_i32, error);
	if (value_i32 < 0 || value_i32 > UINT8_MAX) {
		n_error_set(error, ni_a_errors.NumberOutOfBounds, NULL);
		return;
	}
	*value = value_i32;
	return;
}


static void
consume_double(NLexer* lexer, double* value, NError* error) {
	char* end_ptr;
	NToken cur_token;
	int stored_errno = 0;
	expect_token_type(lexer, NI_TK_REAL, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = strtod(cur_token.lexeme, &end_ptr);
	stored_errno = errno;
	ni_destruct_token(&cur_token);
	assert(*end_ptr == '\0');
	assert(stored_errno != ERANGE);
}


static void
consume_label(NLexer* lexer, char** value, NError* error) {
	NToken cur_token;
	expect_token_type(lexer, NI_TK_LABEL, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = strdup(&cur_token.lexeme[1]);
	ni_destruct_token(&cur_token);
}


static uint8_t
consume_opcode(NLexer* lexer, NError* error) {
	NTokenType token_type = ni_lexer_peek(lexer);
	if (!ni_token_is_opcode(token_type)) {
		n_error_set(error, ni_a_errors.reader.UnexpectedToken, NULL);
		return 0;
	}
	ni_lexer_advance(lexer);
	switch (token_type) {
		#define X( _, op_token, op_code ) case op_token: return op_code;
		NI_OPCODE_MAPPINGS
		default:
			n_error_set(error, ni_a_errors.reader.UnimplementedOpcode, NULL);
			return 0;
	}

}


static void
consume_string(NLexer* lexer, char** value, size_t* length, NError* error) {
	NToken cur_token;
	expect_token_type(lexer, NI_TK_STRING, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = parse_string(cur_token, length);
	ni_destruct_token(&cur_token);
}

