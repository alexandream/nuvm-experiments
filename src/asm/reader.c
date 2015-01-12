#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "reader.h"

#include "common/polyfills/p-strdup.h"

#define CREG 1
#define GREG 2
#define LREG 4

static uint32_t error_eof,
                error_incompatible_register_type,
                error_register_out_of_range,
                error_unexpected_token;

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
consume_string(NLexer* lexer, char** value, NError* error);

static int32_t
parse_dec_integer(const char* lexeme);

static NInstructionArgument
parse_register(NLexer* lexer, uint8_t bits, uint8_t types, NError* error);

static void
read_register_based_instruction(NLexer* lexer,
                                NInstructionDescriptor* instruction,
                                uint8_t reg_a_bits,
                                uint8_t reg_a_types,
                                uint8_t reg_b_bits,
                                uint8_t reg_b_types,
                                uint8_t reg_c_bits,
                                uint8_t reg_c_types,
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
                        NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_STRING, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_string(lexer, value, error);
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
                           NError* error) {
	n_error_reset(error);
	expect_token_type(lexer, NI_TK_KW_CHARACTER, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_string(lexer, value, error);
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
                    NInstructionDescriptor* instruction,
                    NError* error) {

}

void
ni_read_arithmetic_rel_instruction(NLexer* lexer,
                               NInstructionDescriptor* instruction,
                               NError* error) {
	read_register_based_instruction(lexer, instruction,
	                                8,  LREG,
	                                7,  LREG|CREG,
									7,  LREG|CREG,
	                                error);
}


void
ni_read_move_instruction(NLexer* lexer,
                         NInstructionDescriptor* instruction,
                         NError* error) {
	read_register_based_instruction(lexer, instruction,
	                                8,  LREG,
	                               15,  LREG|CREG,
	                                0,  0,
	                                error);
}


static void
read_register_based_instruction(NLexer* lexer,
                                NInstructionDescriptor* instruction,
                                uint8_t reg_a_bits,
                                uint8_t reg_a_types,
                                uint8_t reg_b_bits,
                                uint8_t reg_b_types,
                                uint8_t reg_c_bits,
                                uint8_t reg_c_types,
                                NError* error) {
	uint8_t opcode = consume_opcode(lexer, error);
	if (!n_error_ok(error)) return;

	instruction->opcode = opcode;

	if (reg_a_bits > 0) {
		instruction->arg_a = parse_register(lexer,
		                                    reg_a_bits,
		                                    reg_a_types,
		                                    error);
		if (!n_error_ok(error)) return;
	}

	if (reg_b_bits > 0) {
		instruction->arg_b = parse_register(lexer,
		                                    reg_b_bits,
		                                    reg_b_types,
		                                    error);
		if (!n_error_ok(error)) return;
	}

	if (reg_c_bits > 0) {
		instruction->arg_c = parse_register(lexer,
		                                    reg_c_bits,
		                                    reg_c_types,
		                                    error);
		if (!n_error_ok(error)) return;
	}
}




static NInstructionArgument
parse_register(NLexer* lexer, uint8_t bits, uint8_t types, NError* error) {
	NInstructionArgument result;
	NToken token = NI_TOKEN_INITIALIZER;
	int32_t max_value, value;
	uint8_t type;

	expect_token_type(lexer, NI_TK_REGISTER, error);
	if (!n_error_ok(error)) goto handle_error;

	token = ni_lexer_read(lexer);

	switch(bits) {
		case 7:
			max_value = 127;
			break;
		case 8:
			max_value = 255;
			break;
		case 15:
			max_value = 32767;
			break;
		case 16:
			max_value = 65535;
			break;
		case 22:
			max_value = 4194303;
			break;
		default:
			/* This shouldn't happen. Just a guard here. */
			exit(-1);
	}

	switch(token.lexeme[0]) {
		case 'C':
			type = CREG;
			break;
		case 'G':
			type = GREG;
			break;
		case 'L':
			type = LREG;
			break;
		default:
			/* This shouldn't happen. Just a guard here. */
			exit(-1);
	}

	if (! (types & type)) {
		/* TODO: Put more information in this error result. */
		/* What type did we find? Which are acceptable? Which argument? */
		error->type = error_incompatible_register_type;
		goto handle_error;
	}

	value = parse_dec_integer(token.lexeme+2);
	if (value > max_value) {
		/* TODO: Put more information in this error result. */
		/* What value did we find? What range is ok? Which argument? */
		error->type = error_register_out_of_range;
		goto handle_error;
	}
	result.type = type;
	result.value = value;

	ni_destroy_token(token);
	return result;
handle_error:
	ni_destroy_token(token);
	return result;
}


void
ni_init_reader() {
	error_eof =
		n_register_error_type("nuvm.asm.reader.EOF", NULL, NULL);

	assert(error_eof < N_ERROR_LAST_VALID_ERROR);

	error_unexpected_token =
		n_register_error_type("nuvm.asm.reader.UnexpectedToken", NULL, NULL);

	assert(error_unexpected_token < N_ERROR_LAST_VALID_ERROR);

	error_incompatible_register_type =
		n_register_error_type("nuvm.asm.reader.IncompatibleRegisterType",
		                      NULL,
		                      NULL);

	assert(error_incompatible_register_type < N_ERROR_LAST_VALID_ERROR);

	error_register_out_of_range =
		n_register_error_type("nuvm.asm.reader.RegisterOutOfRange",
		                      NULL,
		                      NULL);

	assert(error_register_out_of_range < N_ERROR_LAST_VALID_ERROR);
}



static void
expect_token_type(NLexer* lexer, NTokenType expected_type, NError* error) {
	NTokenType cur_token_type = ni_lexer_peek(lexer);
	n_error_reset(error);

	if (cur_token_type != expected_type) {
		/* TODO (#1): Generate proper error. */
		error->type = 1;
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


static char*
parse_string(NToken token) {
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
			/* TODO (#2): Implement proper escaping, right now we only handle the
			 * escaped quote (\") case correctly. */
			i++;
		}
		result[j] = input[i];
		j++;
	}
	return result;
}


static void
consume_dec_int(NLexer* lexer, int32_t* value, NError* error) {
	NToken cur_token = NI_TOKEN_INITIALIZER;
	expect_token_type(lexer, NI_TK_DEC_INTEGER, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = parse_dec_integer(cur_token.lexeme);
	ni_destroy_token(cur_token);
}


static void
consume_dec_int_u16(NLexer* lexer, uint16_t* value, NError* error) {
	int32_t value_i32;
	consume_dec_int(lexer, &value_i32, error);
	if (value_i32 < 0 || value_i32 > UINT16_MAX) {
		/* TODO (#1): Generate proper error. */
		error->type = 1;
		return;
	}
	*value = value_i32;
	return;
}


static void
consume_dec_int_u8(NLexer* lexer, uint8_t* value, NError* error) {
	int32_t value_i32;
	consume_dec_int(lexer, &value_i32, error);
	if (value_i32 < 0 || value_i32 > UINT8_MAX) {
		/* TODO (#1): Generate proper error. */
		error->type = 1;
		return;
	}
	*value = value_i32;
	return;
}


static void
consume_double(NLexer* lexer, double* value, NError* error) {
	char* end_ptr;
	NToken cur_token = NI_TOKEN_INITIALIZER;
	expect_token_type(lexer, NI_TK_REAL, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = strtod(cur_token.lexeme, &end_ptr);
	ni_destroy_token(cur_token);
	assert(*end_ptr == '\0');
	assert(errno != ERANGE);
}


static void
consume_label(NLexer* lexer, char** value, NError* error) {
	NToken cur_token = NI_TOKEN_INITIALIZER;
	expect_token_type(lexer, NI_TK_LABEL, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = strdup(&cur_token.lexeme[1]);
	ni_destroy_token(cur_token);
}


static uint8_t
consume_opcode(NLexer* lexer, NError* error) {
	return 0;
}


static void
consume_string(NLexer* lexer, char** value, NError* error) {
	NToken cur_token = NI_TOKEN_INITIALIZER;
	expect_token_type(lexer, NI_TK_STRING, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = parse_string(cur_token);
	ni_destroy_token(cur_token);
}

