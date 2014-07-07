#define SUITE_NAME Lexer
#include <string.h>

#include "../../test-suite.h"

#include "lexer.h"

static ni_lexer_t* LEXER = NULL;

static void
ADVANCE();

static bool
STRINGS_EQUAL(const char* str1, const char* str2);

static void
WITH_INPUT(const char* str);


#define ASSERT_PEEK(_type, _lexeme) do{\
	ni_token_type_t type = _type;\
	const char* lexeme = _lexeme;\
	ni_token_t token = ni_lexer_peek(LEXER);\
	ASSERT_MSG(token.type == type && STRINGS_EQUAL(token.lexeme, lexeme),\
		MF("Expected token type %d with lexeme %s. "\
		   "Got token type %d with lexeme %s",\
		   type, lexeme, token.type, token.lexeme));\
} while(0)


TEARDOWN {
	if (LEXER != NULL) {
		ni_destroy_lexer(LEXER);
		LEXER = NULL;
	}
}


TEST(peek_reads_first_token) {
	WITH_INPUT("hello");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "hello");
}


TEST(peek_reads_eof_on_empty) {
	WITH_INPUT("");
	ASSERT_PEEK(NI_TK_EOF, "");
}


TEST(double_peek_returns_first_token_twice) {
	WITH_INPUT("hello world");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "hello");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "hello");
}


TEST(peek_advance_peek_reads_first_two_tokens) {
	WITH_INPUT("hello world");
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "hello");
	ADVANCE();
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "world");
}


TEST(advance_peek_reads_second_token) {
	WITH_INPUT("hello world");
	ADVANCE();
	ASSERT_PEEK(NI_TK_UNRECOGNIZED_OPCODE, "world");
}


TEST(peek_reads_eof_after_advancing_to_end) {
	WITH_INPUT("hello");
	ADVANCE();
	ASSERT_PEEK(NI_TK_EOF, "");
}


TEST(peek_reads_eof_after_advancing_past_end) {
	WITH_INPUT("hello");
	ADVANCE();
	ADVANCE();
	ASSERT_PEEK(NI_TK_EOF, "");
}



static void
ADVANCE() {
	ni_lexer_advance(LEXER);
}


static bool
STRINGS_EQUAL(const char* str1, const char* str2) {
	return (str1 == NULL && str2 == NULL) ||
	       (str1 != NULL && str2 != NULL && strcmp(str1, str2) == 0);
}

static void
WITH_INPUT(const char* str) {
	LEXER = ni_new_lexer(ni_new_stream_from_string(str));
}
