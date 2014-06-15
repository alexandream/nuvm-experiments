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

	S_REAL_PREFIX,
	S_REAL,

	S_IDENTIFIER,
	S_LABEL_DEFINITION,

	S_REGISTER,
	S_REGISTER_LEAD,
	S_REGISTER_PREFIX,

	S_STRING_OPENED,
	S_STRING_END,
	S_STRING_ESCAPE,

	S_KEYWORD,

	S_UNKNOWN = -1
} tk_state_t;


static n_token_type_t
compute_token_type_from_keyword(const char* keyword);

static n_token_type_t
compute_token_type_from_state(tk_state_t state);

static void
feed_store(store_t* buffer, char chr, bool* overflow);

static tk_state_t
handle_S_INIT(char chr, bool* handling_spaces);

static tk_state_t
handle_S_IDENTIFIER(char chr);

static tk_state_t
handle_S_LEADING_ZERO(char chr);

static tk_state_t
handle_S_DECIMAL_NUMBER(char chr);

static tk_state_t
handle_S_HEXADECIMAL_PREFIX(char chr);

static tk_state_t
handle_S_HEXADECIMAL_NUMBER(char chr);

static tk_state_t
handle_S_REAL_PREFIX(char chr);

static tk_state_t
handle_S_REAL(char chr);

static tk_state_t
handle_S_KEYWORD(char chr);

static tk_state_t
handle_S_REGISTER_LEAD(char chr);

static tk_state_t
handle_S_REGISTER_PREFIX(char chr);

static tk_state_t
handle_S_REGISTER(char chr);

static tk_state_t
handle_S_STRING_OPENED(char chr, bool* handling_spaces, bool* complete);

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


/* The function below implements the following lexer, in POSIX Extended Regular
 * Expressions Syntax and top-to-bottom precedence:
 *
 *  [CLG]:[0-9]+                             N_TK_REGISTER
 *  [a-zA-Z][a-zA-Z0-9-]*                    N_TK_IDENTIFIER
 *  [a-zA-Z][a-zA-Z0-9-]*:                   N_TK_LABEL_DEF
 *  "(\\.|[^\"])"                            N_TK_STRING
 *  0|(-?[1-9][0-9]*)                        N_TK_DEC_INTEGER
 *  0x[0-9A-F]+                              N_TK_HEX_INTEGER
 *  (0|(-?[1-9][0-9]*))\.[0-9]+              N_TK_REAL
 *  \..*                                     N_TK_UNRECOGNIZED_KW
 *
 *  Unrecognized Keywords are further passed into the function
 *  compute_token_type_from_keyword to decide which valid keyword token, if any
 *  is represented by the lexeme.
 *
 *  A graphical representation of the Finite State Machine implemented by this
 *  function is given in
 *
 *    docs/diagrams/tokenizer-state-machine.{svg,dia}
 */
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
				state = handle_S_INIT(chr, &handling_spaces);
				break;
			case S_IDENTIFIER:
				state = handle_S_IDENTIFIER(chr);
				break;
			case S_LEADING_ZERO:
				state = handle_S_LEADING_ZERO(chr);
				break;
			case S_DECIMAL_NUMBER:
				state = handle_S_DECIMAL_NUMBER(chr);
				break;
			case S_HEXADECIMAL_PREFIX:
				state = handle_S_HEXADECIMAL_PREFIX(chr);
				break;
			case S_HEXADECIMAL_NUMBER:
				state = handle_S_HEXADECIMAL_NUMBER(chr);
				break;
			case S_REAL_PREFIX:
				state = handle_S_REAL_PREFIX(chr);
				break;
			case S_REAL:
				state = handle_S_REAL(chr);
				break;
			case S_KEYWORD:
				state = handle_S_KEYWORD(chr);
				break;
			case S_LABEL_DEFINITION:
				state = S_UNKNOWN;
				break;
			case S_REGISTER_LEAD:
				state = handle_S_REGISTER_LEAD(chr);
				break;
			case S_REGISTER_PREFIX:
				state = handle_S_REGISTER_PREFIX(chr);
				break;
			case S_REGISTER:
				state = handle_S_REGISTER(chr);
				break;
			case S_STRING_OPENED:
				state =
					handle_S_STRING_OPENED(chr, &handling_spaces, &complete);
				break;
			case S_STRING_ESCAPE:
				state = S_STRING_OPENED;
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
		case S_REAL:
			return N_TK_REAL;
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


static tk_state_t
handle_S_INIT(char chr, bool* handling_spaces) {
	if (chr == 'L' || chr == 'G' || chr == 'C') {
		return S_REGISTER_LEAD;
	}
	if (isalpha(chr)) {
		return S_IDENTIFIER;
	}
	if (chr == '.') {
		return S_KEYWORD;
	}
	if (chr == '0') {
		return S_LEADING_ZERO;
	}
	if (isdigit(chr) || chr == '-') {
		return S_DECIMAL_NUMBER;
	}
	if (chr == '"') {
		*handling_spaces = true;
		return S_STRING_OPENED;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_IDENTIFIER(char chr) {
	if (isalnum(chr) || chr == '-') {
		return S_IDENTIFIER;
	}
	if (chr == ':') {
		return S_LABEL_DEFINITION;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_LEADING_ZERO(char chr) {
	if (isdigit(chr)) {
		return S_DECIMAL_NUMBER;
	}
	if (chr == 'x') {
		return S_HEXADECIMAL_PREFIX;
	}
	if (chr == '.') {
		return S_REAL_PREFIX;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_DECIMAL_NUMBER(char chr) {
	if (chr == '.') {
		return S_REAL_PREFIX;
	}
	if (isdigit(chr)) {
		return S_DECIMAL_NUMBER;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_HEXADECIMAL_PREFIX(char chr) {
	if (isxdigit(chr)) {
		return S_HEXADECIMAL_NUMBER;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_HEXADECIMAL_NUMBER(char chr) {
	if (isxdigit(chr)) {
		return S_HEXADECIMAL_NUMBER;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_REAL_PREFIX(char chr) {
	if (isdigit(chr)) {
		return S_REAL;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_REAL(char chr) {
	if (isdigit(chr)) {
		return S_REAL;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_KEYWORD(char chr) {
	if (islower(chr) || isdigit(chr) || chr == '-') {
		return S_KEYWORD;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_REGISTER_LEAD(char chr) {
	if (isalnum(chr) || chr == '-') {
		return S_IDENTIFIER;
	}
	if (chr == ':') {
		return S_REGISTER_PREFIX;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_REGISTER_PREFIX(char chr) {
	if (isdigit(chr)) {
		return S_REGISTER;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_REGISTER(char chr) {
	if (isdigit(chr)) {
		return S_REGISTER;
	}
	return S_UNKNOWN;
}


static tk_state_t
handle_S_STRING_OPENED(char chr, bool* handling_spaces, bool* complete) {
	if (chr == '\n') {
		*complete = true;
		return S_UNKNOWN;
	}
	if (chr == '\\') {
		return S_STRING_ESCAPE;
	}
	if (chr == '"') {
		*handling_spaces = false;
		return S_STRING_END;
	}
	return S_STRING_OPENED;
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
