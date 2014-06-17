#define SUITE_NAME Tokens
#include <string.h>

#include "../../test-suite.h"

#include "streams.h"
#include "tokens.h"

static bool
STRINGS_EQUAL(const char* str1, const char* str2);

static void 
WITH_STREAM(const char* str);

static void
END_STREAM();

#define ASSERT_EOF() do {\
	n_token_t token = n_get_next_token(STREAM);\
	ASSERT_MSG(token.type == N_TK_EOF,\
		MF("Expected EOF, got token type %u with lexeme %s.",\
		   token.type, token.lexeme));\
} while(0)

#define ASSERT_TOKEN(_type, _lexeme) do{\
	n_token_type_t type = _type;\
	const char* lexeme = _lexeme;\
	n_token_t token = n_get_next_token(STREAM);\
	ASSERT_MSG(token.type == type && STRINGS_EQUAL(token.lexeme, lexeme),\
		MF("Expected token type %d with lexeme %s. "\
		   "Got token type %d with lexeme %s",\
		   type, lexeme, token.type, token.lexeme));\
} while(0)

static n_stream_t* STREAM = NULL;


TEST(ignores_only_spaces) {
	WITH_STREAM("  \n \t \n \n \t  ");
	ASSERT_EOF();
}


TEST(reads_single_character_identifier) {
	WITH_STREAM("h");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "h");
	ASSERT_EOF();
}


TEST(reads_word_identifier) {
	WITH_STREAM("hello");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "hello");
	ASSERT_EOF();
}


TEST(reads_case_sensitive_identifier) {
	WITH_STREAM("hElLo");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "hElLo");
	ASSERT_EOF();
}


TEST(reads_identifier_with_digits) {
	WITH_STREAM("h1234");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "h1234");
	ASSERT_EOF();
}


TEST(reads_identifier_with_hyphen) {
	WITH_STREAM("hello-world");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "hello-world");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_local_register_lead) {
	WITH_STREAM("Lambda");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "Lambda");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_global_register_lead) {
	WITH_STREAM("Gambling");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "Gambling");
	ASSERT_EOF();
}


TEST(reads_identifier_with_starting_constant_register_lead) {
	WITH_STREAM("Command");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "Command");
	ASSERT_EOF();
}


TEST(ignores_spaces) {
	WITH_STREAM(" \n\t h1234 \t\n ");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "h1234");
	ASSERT_EOF();
}


TEST(reads_many_identifiers) {
	WITH_STREAM("  hello world-o \t g00   ");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "hello");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "world-o");
	ASSERT_TOKEN(N_TK_IDENTIFIER, "g00");
	ASSERT_EOF();
}


TEST(reads_zero_decimal_integer) {
	WITH_STREAM("  0 ");
	ASSERT_TOKEN(N_TK_DEC_INTEGER, "0");
	ASSERT_EOF();
}


TEST(reads_decimal_integer_with_leading_zero) {
	WITH_STREAM(" 01234 ");
	ASSERT_TOKEN(N_TK_DEC_INTEGER, "01234");
	ASSERT_EOF();
}


TEST(reads_decimal_integer) {
	WITH_STREAM(" 10588 ");
	ASSERT_TOKEN(N_TK_DEC_INTEGER, "10588");
	ASSERT_EOF();
}


TEST(reads_negative_decimal_integer) {
	WITH_STREAM("-123456789");
	ASSERT_TOKEN(N_TK_DEC_INTEGER, "-123456789");
	ASSERT_EOF();
}


TEST(reads_single_digit_decimal_integer) {
	WITH_STREAM("8");
	ASSERT_TOKEN(N_TK_DEC_INTEGER, "8");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer) {
	WITH_STREAM("0x12345678");
	ASSERT_TOKEN(N_TK_HEX_INTEGER, "0x12345678");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer_high_digits) {
	WITH_STREAM("0xdeadbeef");
	ASSERT_TOKEN(N_TK_HEX_INTEGER, "0xdeadbeef");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_integer_case_sensitive_high_digits) {
	WITH_STREAM("0xDeadBeef");
	ASSERT_TOKEN(N_TK_HEX_INTEGER, "0xDeadBeef");
	ASSERT_EOF();
}

TEST(reads_hexadecimal_single_digit) {
	WITH_STREAM("0xA");
	ASSERT_TOKEN(N_TK_HEX_INTEGER, "0xA");
	ASSERT_EOF();
}


TEST(reads_empty_string) {
	WITH_STREAM("\"\"");
	ASSERT_TOKEN(N_TK_STRING, "\"\"");
	ASSERT_EOF();
}


TEST(reads_string) {
	WITH_STREAM("\"hello\"");
	ASSERT_TOKEN(N_TK_STRING, "\"hello\"");
	ASSERT_EOF();
}


TEST(reads_string_with_spaces) {
	WITH_STREAM("\"  hello, world   \"");
	ASSERT_TOKEN(N_TK_STRING, "\"  hello, world   \"");
	ASSERT_EOF();
}


TEST(reads_string_with_inner_quotes) {
	WITH_STREAM("\"  hello, \\\"world  \"");
	ASSERT_TOKEN(N_TK_STRING, "\"  hello, \\\"world  \"");
	ASSERT_EOF();
}


TEST(reads_label_def) {
	WITH_STREAM("my-label:");
	ASSERT_TOKEN(N_TK_LABEL_DEF, "my-label:");
	ASSERT_EOF();
}


TEST(reads_local_register_reference_zero) {
	WITH_STREAM("L:0");
	ASSERT_TOKEN(N_TK_REGISTER, "L:0");
	ASSERT_EOF();
}


TEST(reads_global_register_reference_zero) {
	WITH_STREAM("G:0");
	ASSERT_TOKEN(N_TK_REGISTER, "G:0");
	ASSERT_EOF();
}


TEST(reads_constant_register_reference_zero) {
	WITH_STREAM("C:0");
	ASSERT_TOKEN(N_TK_REGISTER, "C:0");
	ASSERT_EOF();
}


TEST(reads_local_register_reference) {
	WITH_STREAM("L:1234");
	ASSERT_TOKEN(N_TK_REGISTER, "L:1234");
	ASSERT_EOF();
}


TEST(reads_global_register_reference) {
	WITH_STREAM("G:4321");
	ASSERT_TOKEN(N_TK_REGISTER, "G:4321");
	ASSERT_EOF();
}


TEST(reads_constant_register_reference) {
	WITH_STREAM("C:987");
	ASSERT_TOKEN(N_TK_REGISTER, "C:987");
	ASSERT_EOF();
}


TEST(reads_keyword_character) {
	WITH_STREAM(".character");
	ASSERT_TOKEN(N_TK_KW_CHARACTER, ".character");
	ASSERT_EOF();
}

TEST(reads_keyword_code) {
	WITH_STREAM(".code");
	ASSERT_TOKEN(N_TK_KW_CODE, ".code");
	ASSERT_EOF();
}


TEST(reads_keyword_constants) {
	WITH_STREAM(".constants");
	ASSERT_TOKEN(N_TK_KW_CONSTANTS, ".constants");
	ASSERT_EOF();
}


TEST(reads_keyword_double) {
	WITH_STREAM(".double");
	ASSERT_TOKEN(N_TK_KW_DOUBLE, ".double");
	ASSERT_EOF();
}


TEST(reads_keyword_globals_count) {
	WITH_STREAM(".globals-count");
	ASSERT_TOKEN(N_TK_KW_GLOBALS_COUNT, ".globals-count");
	ASSERT_EOF();
}


TEST(reads_keyword_int32) {
	WITH_STREAM(".int32");
	ASSERT_TOKEN(N_TK_KW_INT32, ".int32");
	ASSERT_EOF();
}


TEST(reads_keyword_procedure) {
	WITH_STREAM(".procedure");
	ASSERT_TOKEN(N_TK_KW_PROCEDURE, ".procedure");
	ASSERT_EOF();
}


TEST(reads_keyword_string) {
	WITH_STREAM(".string");
	ASSERT_TOKEN(N_TK_KW_STRING, ".string");
	ASSERT_EOF();
}


TEST(reads_keyword_version) {
	WITH_STREAM(".version");
	ASSERT_TOKEN(N_TK_KW_VERSION, ".version");
	ASSERT_EOF();
}


TEST(reads_unrecognized_keyword) {
	WITH_STREAM(".whatever-we-want-2");
	ASSERT_TOKEN(N_TK_UNRECOGNIZED_KW, ".whatever-we-want-2");
	ASSERT_EOF();
}


TEST(reads_real_zero) {
	WITH_STREAM("0.0");
	ASSERT_TOKEN(N_TK_REAL, "0.0");
	ASSERT_EOF();
}


TEST(reads_real_positive_number) {
	WITH_STREAM("3.1415927");
	ASSERT_TOKEN(N_TK_REAL, "3.1415927");
	ASSERT_EOF();
}


TEST(reads_real_negative_number) {
	WITH_STREAM("-10.588");
	ASSERT_TOKEN(N_TK_REAL, "-10.588");
	ASSERT_EOF();
}


TEST(reads_label_reference) {
	WITH_STREAM("@the-label");
	ASSERT_TOKEN(N_TK_LABEL, "@the-label");
	ASSERT_EOF();
}


static void 
WITH_STREAM(const char* str) {
	END_STREAM();
	STREAM = n_new_stream_from_string(str);
}


static void
END_STREAM() {
	if (STREAM) {
		n_destroy_stream(STREAM);
		STREAM = NULL;
	}
}

static bool
STRINGS_EQUAL(const char* str1, const char* str2) {
	return (str1 == NULL && str2 == NULL) ||
	       (str1 != NULL && str2 != NULL && strcmp(str1, str2) == 0);
}
