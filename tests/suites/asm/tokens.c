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


TEST(reads_zero_decimal_number) {
	WITH_STREAM("  0 ");
	ASSERT_TOKEN(N_TK_DECNUM, "0");
	ASSERT_EOF();
}


TEST(reads_decimal_number_with_leading_zero) {
	WITH_STREAM(" 01234 ");
	ASSERT_TOKEN(N_TK_DECNUM, "01234");
	ASSERT_EOF();
}


TEST(reads_decimal_number) {
	WITH_STREAM(" 10588 ");
	ASSERT_TOKEN(N_TK_DECNUM, "10588");
	ASSERT_EOF();
}


TEST(reads_negative_decimal_number) {
	WITH_STREAM("-123456789");
	ASSERT_TOKEN(N_TK_DECNUM, "-123456789");
	ASSERT_EOF();
}


TEST(reads_single_digit_decimal_number) {
	WITH_STREAM("8");
	ASSERT_TOKEN(N_TK_DECNUM, "8");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_number) {
	WITH_STREAM("0x12345678");
	ASSERT_TOKEN(N_TK_HEXNUM, "0x12345678");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_number_high_digits) {
	WITH_STREAM("0xdeadbeef");
	ASSERT_TOKEN(N_TK_HEXNUM, "0xdeadbeef");
	ASSERT_EOF();
}


TEST(reads_hexadecimal_number_case_sensitive_high_digits) {
	WITH_STREAM("0xDeadBeef");
	ASSERT_TOKEN(N_TK_HEXNUM, "0xDeadBeef");
	ASSERT_EOF();
}

TEST(reads_hexadecimal_single_digit) {
	WITH_STREAM("0xA");
	ASSERT_TOKEN(N_TK_HEXNUM, "0xA");
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
