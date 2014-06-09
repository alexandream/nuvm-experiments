#include <ctype.h>
#include <stdlib.h>

#include "tokens.h"

#include "common/polyfills/strdup.h"

#ifndef LEXEME_BUFFER_SIZE
#define LEXEME_BUFFER_SIZE 256
#endif

typedef struct {
	char* store;
	uint16_t size;
	uint16_t capacity;
} store_t;

typedef enum {
	S_UNKNOWN = 0,
	S_INIT,
	S_IDENTIFIER,
	S_LEADING_ZERO,
	S_DECIMAL_NUMBER
} tk_state_t;


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
		 overflow = false;
	int consumed_size = 0;
	char chr;
	tk_state_t state = S_INIT;

	init_store(&store, buffer, LEXEME_BUFFER_SIZE);

	ignore_whitespace(stream, &eof);
	chr = n_stream_peek(stream, &eof);
	while (!eof && !overflow) {
		switch (state) {
			case S_INIT:
				feed_store(&store, chr, &overflow);
				if (isalpha(chr)) {
					state = S_IDENTIFIER;
				}
				else if (chr == '0') {
					state = S_LEADING_ZERO;
				}
				else if (isdigit(chr) || chr == '-') {
					state = S_DECIMAL_NUMBER;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_IDENTIFIER:
				feed_store(&store, chr, &overflow);
				if (!isalnum(chr) && chr != '-') {
					state = S_UNKNOWN;
				}
				break;
			case S_LEADING_ZERO:
				feed_store(&store, chr, &overflow);
				if (isdigit(chr)) {
					state = S_DECIMAL_NUMBER;
				}
				else {
					state = S_UNKNOWN;
				}
				break;
			case S_DECIMAL_NUMBER:
				feed_store(&store, chr, &overflow);
				if (!isdigit(chr)) {
					state = S_UNKNOWN;
				}
				break;
			case S_UNKNOWN:
				feed_store(&store, chr, &overflow);
				break;
		}
		n_stream_read(stream, &eof);
		consumed_size++;
		chr = n_stream_peek(stream, &eof);
		if (isspace(chr)) {
			break;
		}
	}
	if ((eof && consumed_size > 0) || (!eof && !overflow)) {
		buffer[consumed_size] = '\0';
		result.type = compute_token_type_from_state(state);
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
compute_token_type_from_state(tk_state_t state) {
	switch (state) {
		case S_IDENTIFIER:
			return N_TK_IDENTIFIER;
		case S_LEADING_ZERO: /* fall-through */
		case S_DECIMAL_NUMBER:
			return N_TK_DECNUM;
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
