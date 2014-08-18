#include <stdlib.h>
#include <assert.h>
#include <errno.h>

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
	if (!n_error_ok(error)) {
		return;
	}
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
	if (!n_error_ok(error)) return;
}


void
ni_read_globals_count(NLexer* lexer,
                      uint16_t* num_globals,
                      NError* error) {
	expect_token_type(lexer, NI_TK_KW_GLOBALS_COUNT, error);
	if (!n_error_ok(error)) return;

	ni_lexer_advance(lexer);

	consume_dec_int_u16(lexer, num_globals, error);
	if (!n_error_ok(error)) return;
}


void
ni_read_string_constant(NLexer* lexer,
                        char** value,
                        NError* error) { }

void
ni_read_double_constant(NLexer* lexer,
                        double* value,
                        NError* error) { }

void
ni_read_character_constant(NLexer* lexer,
						   char** value,
						   NError* error) { }

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
	if (!n_error_ok(error)) return;
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
		/* TODO: Generate proper error. */
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


static void
consume_dec_int(NLexer* lexer, int32_t* value, NError* error) {
	NToken cur_token = NI_TOKEN_INITIALIZER;
	NTokenType cur_token_type = ni_lexer_peek(lexer);
	if (cur_token_type != NI_TK_DEC_INTEGER) {
		/* TODO: generate proper error. */
		return;
	}

	cur_token = ni_lexer_read(lexer);
	*value = parse_dec_integer(cur_token);
	ni_destroy_token(cur_token);
}

static void
consume_dec_int_u16(NLexer* lexer, uint16_t* value, NError* error) {
	int32_t value_i32;
	consume_dec_int(lexer, &value_i32, error);
	if (!n_error_ok(error)) {
		return;
	}
	if (value_i32 < 0 || value_i32 > UINT16_MAX) {
		/* TODO: Generate proper error. */
		return;
	}
	*value = value_i32;
	return;
}


static void
consume_dec_int_u8(NLexer* lexer, uint8_t* value, NError* error) {
	int32_t value_i32;
	consume_dec_int(lexer, &value_i32, error);
	if (!n_error_ok(error)) {
		return;
	}
	if (value_i32 < 0 || value_i32 > UINT8_MAX) {
		/* TODO: Generate proper error. */
		return;
	}
	*value = value_i32;
	return;
}

