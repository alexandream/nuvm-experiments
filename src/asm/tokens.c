#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#include "common/polyfills/strdup.h"

#ifndef LEXEME_BUFFER_SIZE
#define LEXEME_BUFFER_SIZE 256
#endif

typedef struct {
	char* lexeme;
	ni_token_type_t token_type;
} lexeme_table_t;

static lexeme_table_t KEYWORD_TABLE[] = {
	{ ".character",     NI_TK_KW_CHARACTER },
	{ ".code",          NI_TK_KW_CODE },
	{ ".constants",     NI_TK_KW_CONSTANTS },
	{ ".double",        NI_TK_KW_DOUBLE },
	{ ".entry-point",   NI_TK_KW_ENTRY_POINT },
	{ ".globals-count", NI_TK_KW_GLOBALS_COUNT },
	{ ".int32",         NI_TK_KW_INT32 },
	{ ".procedure",     NI_TK_KW_PROCEDURE },
	{ ".string",        NI_TK_KW_STRING },
	{ ".version",       NI_TK_KW_VERSION },
	{   NULL,           NI_TK_UNRECOGNIZED_KW }
};

static lexeme_table_t OPCODE_TABLE[] = {
	{ "move",       NI_TK_OP_MOVE },
	{ "global-ref", NI_TK_OP_GLOBAL_REF },
	{ "global-set", NI_TK_OP_GLOBAL_SET },
	{ "jump",       NI_TK_OP_JUMP },
	{ "jump-if",    NI_TK_OP_JUMP_IF },
	{ "eq",         NI_TK_OP_EQ },
	{ "lt",         NI_TK_OP_LT },
	{ "le",         NI_TK_OP_LE },
	{ "gt",         NI_TK_OP_GT },
	{ "ge",         NI_TK_OP_GE },
	{ "not",        NI_TK_OP_NOT },
	{ "or",         NI_TK_OP_OR },
	{ "and",        NI_TK_OP_AND },
	{ "add",        NI_TK_OP_ADD },
	{ "sub",        NI_TK_OP_SUB },
	{ "mul",        NI_TK_OP_MUL },
	{ "div",        NI_TK_OP_DIV },
	{ "load-bool",  NI_TK_OP_LOAD_BOOL },
	{  NULL,        NI_TK_UNRECOGNIZED_OPCODE }
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
	S_LABEL_LEAD,
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

static ni_token_type_t
adjust_identifier_token_type(const char* lexeme, bool is_label);

static ni_token_type_t
compute_token_type_from_keyword(const char* keyword);

static ni_token_type_t
compute_token_type_from_state(tk_state_t state);

static void
feed_store(store_t* buffer, char chr, bool* overflow);

static tk_state_t
handle_S_INIT(char chr, bool* handling_spaces, bool* is_label);

static tk_state_t
handle_S_IDENTIFIER(char chr, bool is_label);

static tk_state_t
handle_S_LABEL_LEAD(char chr);

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
ignore_whitespace(ni_stream_t* stream, bool* end);

static void
init_store(store_t* buffer, char* store, uint16_t capacity);


void
ni_destroy_token(ni_token_t token) {
	if (token.lexeme != NULL) {
		free(token.lexeme);
	}
}


/* The function below implements the following lexer, in POSIX Extended Regular
 * Expressions Syntax and top-to-bottom precedence:
 *
 *  [CLG]:[0-9]+                             NI_TK_REGISTER
 *  [a-zA-Z][a-zA-Z0-9-]*                    NI_TK_UNRECOGNIZED_OPCODE
 *  [a-zA-Z][a-zA-Z0-9-]*:                   NI_TK_LABEL_DEF
 *  "(\\.|[^\"])"                            NI_TK_STRING
 *  0|(-?[1-9][0-9]*)                        NI_TK_DEC_INTEGER
 *  0x[0-9A-F]+                              NI_TK_HEX_INTEGER
 *  (0|(-?[1-9][0-9]*))\.[0-9]+              NI_TK_REAL
 *  \..*                                     NI_TK_UNRECOGNIZED_KW
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
ni_token_t
ni_get_next_token(ni_stream_t* stream) {
	char buffer[LEXEME_BUFFER_SIZE + 1];
	store_t store;
	ni_token_t result;
	bool eof = false,
	     overflow = false,
	     complete = false;
	int consumed_size = 0;
	char chr;
	bool handling_spaces = false;
	bool is_label = false;
	tk_state_t state = S_INIT;

	init_store(&store, buffer, LEXEME_BUFFER_SIZE);

	ignore_whitespace(stream, &eof);
	chr = ni_stream_peek(stream, &eof);
	while (!eof && !overflow && !complete) {
		feed_store(&store, chr, &overflow);
		switch (state) {
			case S_INIT:
				state = handle_S_INIT(chr, &handling_spaces, &is_label);
				break;
			case S_IDENTIFIER:
				state = handle_S_IDENTIFIER(chr, is_label);
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
			case S_LABEL_LEAD:
				state = handle_S_LABEL_LEAD(chr);
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
			ni_stream_read(stream, &eof);
			consumed_size++;
			chr = ni_stream_peek(stream, &eof);
			if (!handling_spaces && isspace(chr)) {
				complete = true;
			}
		}
	}
	if ((eof && consumed_size > 0) || (!eof && !overflow)) {
		buffer[consumed_size] = '\0';
		result.type = compute_token_type_from_state(state);
		if (result.type == NI_TK_UNRECOGNIZED_OPCODE) {
			result.type = adjust_identifier_token_type(buffer, is_label);
		}
		else if (result.type == NI_TK_UNRECOGNIZED_KW) {
			result.type = compute_token_type_from_keyword(buffer);
		}
		result.lexeme = strdup(buffer);
	}
	else if (eof) {
		result.type = NI_TK_EOF;
		result.lexeme = NULL;
	}
	else /* overflow */ {
		buffer[LEXEME_BUFFER_SIZE] = '\0';
		result.type = NI_TK_TOO_BIG;
		result.lexeme = buffer;
	}
	return result;
}


static ni_token_type_t
compute_token_type_from_table(const char* lexeme, lexeme_table_t* table) {
	int i = 0;

	while (true) {
		const char *test_keyword = table[i].lexeme;
		if (test_keyword == NULL || strcmp(test_keyword, lexeme) == 0) {
			return table[i].token_type;
		}
		else i++;
	}
}


static ni_token_type_t
adjust_identifier_token_type(const char* lexeme, bool is_label) {
	if (is_label) {
		return NI_TK_LABEL;
	}
	else {
		return compute_token_type_from_table(lexeme, OPCODE_TABLE);
	}
}


static ni_token_type_t
compute_token_type_from_keyword(const char* keyword) {
	return compute_token_type_from_table(keyword, KEYWORD_TABLE);
}

static ni_token_type_t
compute_token_type_from_state(tk_state_t state) {
	switch (state) {
		case S_REGISTER_LEAD: /* fall-through */
		case S_IDENTIFIER:
			return NI_TK_UNRECOGNIZED_OPCODE;
		case S_LEADING_ZERO: /* fall-through */
		case S_DECIMAL_NUMBER:
			return NI_TK_DEC_INTEGER;
		case S_REAL:
			return NI_TK_REAL;
		case S_HEXADECIMAL_NUMBER:
			return NI_TK_HEX_INTEGER;
		case S_LABEL_DEFINITION:
			return NI_TK_LABEL_DEF;
		case S_STRING_END:
			return NI_TK_STRING;
		case S_REGISTER:
			return NI_TK_REGISTER;
		case S_KEYWORD:
			return NI_TK_UNRECOGNIZED_KW;
		default:
			return NI_TK_UNKNOWN;
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
handle_S_INIT(char chr, bool* handling_spaces, bool* is_label) {
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
	if (chr == '@') {
		*is_label = true;
		return S_LABEL_LEAD;
	}

	return S_UNKNOWN;
}


static tk_state_t
handle_S_IDENTIFIER(char chr, bool is_label) {
	if (isalnum(chr) || chr == '-') {
		return S_IDENTIFIER;
	}
	if (!is_label && chr == ':') {
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


static tk_state_t
handle_S_LABEL_LEAD(char chr) {
	if (isalpha(chr)) {
		return S_IDENTIFIER;
	}
	return S_UNKNOWN;
}


static void
ignore_whitespace(ni_stream_t* stream, bool* end) {
	char chr = ni_stream_peek(stream, end);
	while (isspace(chr) && !(*end)) {
		ni_stream_read(stream, end);
		chr = ni_stream_peek(stream, end);
	}
}


static void
init_store(store_t* buffer, char* store, uint16_t capacity) {
	buffer->store = store;
	buffer->capacity = capacity;
	buffer->size = 0;
}
