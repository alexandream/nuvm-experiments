#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "opcodes.h"

#ifndef LEXEME_BUFFER_SIZE
#define LEXEME_BUFFER_SIZE 256
#endif

typedef struct {
	char* lexeme;
	NTokenType token_type;
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
#define X(op_name, op_token, op_code) { op_name,      op_token },
	NI_OPCODE_MAPPINGS
#undef X
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
	S_REGISTER_PREFIX,

	S_STRING_OPENED,
	S_STRING_END,
	S_STRING_ESCAPE,

	S_KEYWORD,

	S_UNKNOWN = -1
} tk_state_t;

static NTokenType
adjust_identifier_token_type(const char* lexeme, bool is_label);

static NTokenType
compute_token_type_from_keyword(const char* keyword);

static NTokenType
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
handle_S_REGISTER_PREFIX(char chr);

static tk_state_t
handle_S_REGISTER(char chr);

static tk_state_t
handle_S_STRING_OPENED(char chr, bool* handling_spaces, bool* complete);

static void
ignore_whitespace(NIStream* stream, bool* end);

static void
ignore_until_eol(NIStream* stream, bool* end);

static void
init_store(store_t* buffer, char* store, uint16_t capacity);


void
ni_destroy_token(NToken token) {
	if (token.lexeme != NULL) {
		free(token.lexeme);
	}
}

const char*
ni_get_token_name(NTokenType type) {
	switch (type) {
		case NI_TK_EOF: return "EOF";
		case NI_TK_TOO_BIG: return "TOO_BIG";

		case NI_TK_DEC_INTEGER: return "DEC_INTEGER";
		case NI_TK_HEX_INTEGER: return "HEX_INTEGER";
		case NI_TK_REAL: return "REAL";
		case NI_TK_UNRECOGNIZED_OPCODE: return "UNRECOGNIZED_OPCODE";
		case NI_TK_LABEL: return "LABEL";
		case NI_TK_LABEL_DEF: return "LABEL_DEF";
		case NI_TK_REGISTER: return "REGISTER";
		case NI_TK_STRING: return "STRING";

		case NI_TK_KW_CHARACTER: return "KW_CHARACTER";
		case NI_TK_KW_CODE: return "KW_CODE";
		case NI_TK_KW_CONSTANTS: return "KW_CONSTANTS";
		case NI_TK_KW_DOUBLE: return "KW_DOUBLE";
		case NI_TK_KW_ENTRY_POINT: return "KW_ENTRY_POINT";
		case NI_TK_KW_GLOBALS_COUNT: return "KW_GLOBALS_COUNT";
		case NI_TK_KW_INT32: return "KW_INT32";
		case NI_TK_KW_PROCEDURE: return "KW_PROCEDURE";
		case NI_TK_KW_STRING: return "KW_STRING";
		case NI_TK_KW_VERSION: return "KW_VERSION";

		case NI_TK_UNRECOGNIZED_KW: return "UNRECOGNIZED_KW";

		case NI_TK_OP_GLOBAL_REF: return "OP_GLOBAL_REF";
		case NI_TK_OP_GLOBAL_SET: return "OP_GLOBAL_SET";
		case NI_TK_OP_CONST_REF: return "OP_CONST_REF";
		case NI_TK_OP_MOVE_IN: return "OP_MOVE_IN";
		case NI_TK_OP_MOVE_OUT: return "OP_MOVE_OUT";

		case NI_TK_OP_JUMP: return "OP_JUMP";
		case NI_TK_OP_JUMP_IF: return "OP_JUMP_IF";

		case NI_TK_OP_EQ: return "OP_EQ";
		case NI_TK_OP_GE: return "OP_GE";
		case NI_TK_OP_GT: return "OP_GT";
		case NI_TK_OP_LE: return "OP_LE";
		case NI_TK_OP_LT: return "OP_LT";

		case NI_TK_OP_NOT: return "OP_NOT";

		case NI_TK_OP_AND: return "OP_AND";
		case NI_TK_OP_OR: return "OP_OR";

		case NI_TK_OP_ADD: return "OP_ADD";
		case NI_TK_OP_DIV: return "OP_DIV";
		case NI_TK_OP_MUL: return "OP_MUL";
		case NI_TK_OP_SUB: return "OP_SUB";

		case NI_TK_OP_LOAD_BOOL: return "OP_LOAD_BOOL";

		case NI_TK_OP_RETURN: return "OP_RETURN";

		case NI_TK_UNKNOWN: return "UNKNOWN";
		/* Control tokens should not appear anywhere else. */
		case NI_TK_XX_OPCODES_START:
		case NI_TK_XX_OPCODES_END:
			return "Control Token";
	}
	/* This should also never happen. */
	return "Unprocessed Token";
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
NTokenType
ni_get_next_token(NIStream* stream, char* buffer, size_t buffer_last_pos) {
	store_t store;
	NTokenType result;
	bool eof = false,
	     overflow = false,
	     complete = false;
	int consumed_size = 0;
	char chr;
	bool handling_spaces = false;
	bool is_label = false;
	tk_state_t state = S_INIT;

	init_store(&store, buffer, buffer_last_pos);
	ignore_whitespace(stream, &eof);
	chr = ni_istream_peek(stream, &eof);
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
			ni_istream_read(stream, &eof);
			consumed_size++;
			chr = ni_istream_peek(stream, &eof);
			if (!handling_spaces && isspace(chr)) {
				complete = true;
			}
		}
	}
	if ((eof && consumed_size > 0) || (!eof && !overflow)) {
		buffer[consumed_size] = '\0';
		result = compute_token_type_from_state(state);
		if (result == NI_TK_UNRECOGNIZED_OPCODE) {
			result = adjust_identifier_token_type(buffer, is_label);
		}
		else if (result == NI_TK_UNRECOGNIZED_KW) {
			result = compute_token_type_from_keyword(buffer);
		}
	}
	else if (eof) {
		result = NI_TK_EOF;
		buffer[0] = '\0';
	}
	else /* overflow */ {
		buffer[buffer_last_pos] = '\0';
		result = NI_TK_TOO_BIG;
	}
	return result;
}


/* Takes a stack allocated token value and turns it into a heap allocated.
 * The lexeme, which is already stored in a heap value, is maintained, so the
 * original stack allocated token should not be modified after being lifted. */
NToken*
ni_token_lift(NToken token) {
	/* FIXME (#1): Double check this allocation when GC comes up. */
	NToken* result = (NToken*) malloc(sizeof(NToken));
	if (result != NULL) {
		result->type = token.type;
		result->lexeme = token.lexeme;
	}
	return result;
}


bool
ni_token_is_constant_keyword(NTokenType token_type) {
	switch(token_type) {
		case NI_TK_KW_CHARACTER:
		case NI_TK_KW_DOUBLE:
		case NI_TK_KW_INT32:
		case NI_TK_KW_PROCEDURE:
		case NI_TK_KW_STRING:
			return true;
		default:
			return false;
	}
}


bool
ni_token_is_opcode(NTokenType token_type) {
	return token_type > NI_TK_XX_OPCODES_START &&
	       token_type < NI_TK_XX_OPCODES_END;
}


static NTokenType
compute_token_type_from_table(const char* lexeme, lexeme_table_t* table) {
	/* TODO: Use a real map-like structure for this. Performance only. */
	int i = 0;

	while (true) {
		const char *test_keyword = table[i].lexeme;
		if (test_keyword == NULL || strcmp(test_keyword, lexeme) == 0) {
			return table[i].token_type;
		}
		else i++;
	}
}


static NTokenType
adjust_identifier_token_type(const char* lexeme, bool is_label) {
	if (is_label) {
		return NI_TK_LABEL;
	}
	else {
		return compute_token_type_from_table(lexeme, OPCODE_TABLE);
	}
}


static NTokenType
compute_token_type_from_keyword(const char* keyword) {
	return compute_token_type_from_table(keyword, KEYWORD_TABLE);
}

static NTokenType
compute_token_type_from_state(tk_state_t state) {
	switch (state) {
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
ignore_whitespace(NIStream* stream, bool* end) {
	char chr = ni_istream_peek(stream, end);
	while ((isspace(chr) || chr == '#') && !(*end)) {
		if (chr == '#') ignore_until_eol(stream, end);
		ni_istream_read(stream, end);
		chr = ni_istream_peek(stream, end);
	}
}


static void
ignore_until_eol(NIStream* stream, bool* end) {
	char chr = ni_istream_peek(stream, end);
	/* FIXME: This code only handles unix-style end-of-lines (\n) */
	while (chr != '\n' && !(*end)) {
		ni_istream_read(stream, end);
		chr = ni_istream_peek(stream, end);
	}
}


static void
init_store(store_t* buffer, char* store, uint16_t capacity) {
	buffer->store = store;
	buffer->capacity = capacity;
	buffer->size = 0;
}
