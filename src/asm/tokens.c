#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#include "common/polyfills/strdup.h"

#ifndef LEXEME_BUFFER_SIZE
#define LEXEME_BUFFER_SIZE 256
#endif

typedef struct {
	char* keyword;
	n_token_type_t token_type;
} keyword_table_t;

static keyword_table_t KEYWORD_TABLE[] = {
	{ ".character",     N_TK_KW_CHARACTER },
	{ ".code",          N_TK_KW_CODE },
	{ ".constants",     N_TK_KW_CONSTANTS },
	{ ".double",        N_TK_KW_DOUBLE },
	{ ".globals-count", N_TK_KW_GLOBALS_COUNT },
	{ ".int32",         N_TK_KW_INT32 },
	{ ".procedure",     N_TK_KW_PROCEDURE },
	{ ".string",        N_TK_KW_STRING },
	{ ".version",       N_TK_KW_VERSION },
	{   NULL,           N_TK_UNRECOGNIZED_KW }
};

typedef struct {
	char* store;
	uint16_t size;
	uint16_t capacity;
} store_t;


typedef enum {
	S_INIT,

	S_DECIMAL_NUMBER,
	S_HEXADECIMAL_NUMBER,
	S_HEXADECIMAL_PREFIX,
	S_LEADING_ZERO,

	S_IDENTIFIER,
	S_LABEL_DEFINITION,

	S_REGISTER,
	S_REGISTER_LEAD,
	S_REGISTER_PREFIX,

	S_STRING_OPENED,
	S_STRING_END,

	S_KEYWORD,

	S_UNKNOWN = -1
} tk_state_t;


static n_token_type_t
compute_token_type_from_keyword(const char* keyword);

static n_token_type_t
compute_token_type_from_state(tk_state_t state);

static void
feed_store(store_t* buffer, char chr, bool* overflow);

static void
ignore_whitespace(n_stream_t* stream, bool* end);

static void
init_store(store_t* buffer, char* store, uint16_t capacity);


void
n_destroy_token(n_token_t token) {
	if (token.lexeme != NULL) {
		free(token.lexeme);
	}
}


n_token_t
n_get_next_token(n_stream_t* stream) {
	char buffer[LEXEME_BUFFER_SIZE + 1];
	store_t store;
	n_token_t result;
	bool eof = false,
	     overflow = false,
	     complete = false;
	int consumed_size = 0;
	char chr;
	bool handling_spaces = false;
	tk_state_t state = S_INIT;

	init_store(&store, buffer, LEXEME_BUFFER_SIZE);

	ignore_whitespace(stream, &eof);
	chr = n_stream_peek(stream, &eof);
	while (!eof && !overflow && !complete) {
		feed_store(&store, chr, &overflow);
		switch (state) {
			case S_INIT:
				if (chr == 'L' || chr == 'G' || chr == 'C') {
					state = S_REGISTER_LEAD;
				}
				else if (isalpha(chr)) {
					state = S_IDENTIFIER;
				}
				else if (chr == '.') {
					state = S_KEYWORD;
				}
				else if (chr == '0') {
					state = S_LEADING_ZERO;
				}
				else if (isdigit(chr) || chr == '-') {
					state = S_DECIMAL_NUMBER;
				}
				else if (chr == '"') {
					state = S_STRING_OPENED;
					handling_spaces = true;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_IDENTIFIER:
				if (!isalnum(chr) && chr != '-') {
					if (chr == ':') {
						state = S_LABEL_DEFINITION;
					}
					else {
						state = S_UNKNOWN;
					}
				}
				break;
			case S_LEADING_ZERO:
				if (isdigit(chr)) {
					state = S_DECIMAL_NUMBER;
				}
				else if (chr == 'x') {
					state = S_HEXADECIMAL_PREFIX;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_DECIMAL_NUMBER:
				if (!isdigit(chr)) {
					state = S_UNKNOWN;
				}
				break;
			case S_HEXADECIMAL_PREFIX:
				if (isxdigit(chr)) {
					state = S_HEXADECIMAL_NUMBER;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_HEXADECIMAL_NUMBER:
				if (!isxdigit(chr)) {
					state = S_UNKNOWN;
				}
				break;
			case S_KEYWORD:
				if (!islower(chr) && !isdigit(chr) && chr != '-') {
					state = S_UNKNOWN;
				}
				break;
			case S_LABEL_DEFINITION:
				state = S_UNKNOWN;
				break;
			case S_REGISTER_LEAD:
				if (isalnum(chr) && chr != '-') {
					state = S_IDENTIFIER;
				}
				else if (chr == ':') {
					state = S_REGISTER_PREFIX;
				}
				break;
			case S_REGISTER_PREFIX:
				if (isdigit(chr)) {
					state = S_REGISTER;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_REGISTER:
				if (!isdigit(chr)) {
					state = S_UNKNOWN;
				}
				break;
			case S_STRING_OPENED:
				if (chr == '\n') {
					state = S_UNKNOWN;
					complete = true;
				}
				else if (chr == '"') {
					state = S_STRING_END;
					handling_spaces = false;
				}
				break;
			case S_STRING_END:
				state = S_UNKNOWN;
				break;
			case S_UNKNOWN:
				break;
		}
		if (!complete) {
			n_stream_read(stream, &eof);
			consumed_size++;
			chr = n_stream_peek(stream, &eof);
			if (!handling_spaces && isspace(chr)) {
				complete = true;
			}
		}
	}
	if ((eof && consumed_size > 0) || (!eof && !overflow)) {
		buffer[consumed_size] = '\0';
		result.type = compute_token_type_from_state(state);
		if (result.type == N_TK_UNRECOGNIZED_KW) {
			result.type = compute_token_type_from_keyword(buffer);
		}
		result.lexeme = strdup(buffer);
	}
	else if (eof) {
		result.type = N_TK_EOF;
		result.lexeme = NULL;
	}
	else /* overflow */ {
		buffer[LEXEME_BUFFER_SIZE] = '\0';
		result.type = N_TK_TOO_BIG;
		result.lexeme = buffer;
	}
	return result;
}


static n_token_type_t
compute_token_type_from_keyword(const char* keyword) {
	int i = 0;

	while (true) {
		const char *test_keyword = KEYWORD_TABLE[i].keyword;
		if (test_keyword == NULL || strcmp(test_keyword, keyword) == 0) {
			return KEYWORD_TABLE[i].token_type;
		}
		else i++;
	}
}

static n_token_type_t
compute_token_type_from_state(tk_state_t state) {
	switch (state) {
		case S_REGISTER_LEAD: /* fall-through */
		case S_IDENTIFIER:
			return N_TK_IDENTIFIER;
		case S_LEADING_ZERO: /* fall-through */
		case S_DECIMAL_NUMBER:
			return N_TK_DEC_INTEGER;
		case S_HEXADECIMAL_NUMBER:
			return N_TK_HEX_INTEGER;
		case S_LABEL_DEFINITION:
			return N_TK_LABEL_DEF;
		case S_STRING_END:
			return N_TK_STRING;
		case S_REGISTER:
			return N_TK_REGISTER;
		case S_KEYWORD:
			return N_TK_UNRECOGNIZED_KW;
		default:
			return N_TK_UNKNOWN;
	}
}


static void
feed_store(store_t* buffer, char chr, bool* overflow) {
	if (buffer->size == buffer->capacity) {
		*overflow = true;
	}
	else {
		buffer->store[buffer->size] = chr;
		buffer->size++;
	}
}


static void
ignore_whitespace(n_stream_t* stream, bool* end) {
	char chr = n_stream_peek(stream, end);
	while (isspace(chr) && !(*end)) {
		n_stream_read(stream, end);
		chr = n_stream_peek(stream, end);
	}
}


static void
init_store(store_t* buffer, char* store, uint16_t capacity) {
	buffer->store = store;
	buffer->capacity = capacity;
	buffer->size = 0;
}
