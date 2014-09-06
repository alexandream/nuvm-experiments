#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "reader.h"

static uint32_t
reader_eof_error;

static void
expect_token_type(NLexer* lexer, NTokenType expected_type, NError* error);

static void
consume_dec_int(NLexer* lexer, int32_t* value, NError* error);

static void
consume_dec_int_u16(NLexer* lexer, uint16_t* value, NError* error);

static void
consume_dec_int_u8(NLexer* lexer, uint8_t* value, NError* error);

static void
consume_string(NLexer* lexer, char** value, NError* error);

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
                        NError* error) { }

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
                           NError* error) { }

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
ni_init_reader() {
	reader_eof_error =
		n_register_error_type("nuvm.asm.reader.errors.EOF", NULL, NULL);

	assert(reader_eof_error < N_ERROR_LAST_VALID_ERROR);
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
parse_dec_integer(NToken token) {
	char* end_ptr;
	int32_t result = strtol(token.lexeme, &end_ptr, 10);
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
	*value = parse_dec_integer(cur_token);
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
consume_string(NLexer* lexer, char** value, NError* error) {
	NToken cur_token = NI_TOKEN_INITIALIZER;
	expect_token_type(lexer, NI_TK_STRING, error);
	if (!n_error_ok(error)) return;

	cur_token = ni_lexer_read(lexer);
	*value = parse_string(cur_token);
	ni_destroy_token(cur_token);
}

