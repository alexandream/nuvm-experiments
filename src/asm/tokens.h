#ifndef __N__ASM__TOKENIZER__H__
#define __N__ASM__TOKENIZER__H__

#include "streams.h"

typedef enum {
	N_TK_EOF = 0,
	N_TK_TOO_BIG,

	N_TK_DECNUM,
	N_TK_HEXNUM,
	N_TK_IDENTIFIER,
	N_TK_LABEL_DEF,
	N_TK_REGISTER,
	N_TK_STRING,

	N_TK_KW_CHARACTER,
	N_TK_KW_CODE,
	N_TK_KW_CONSTANTS,
	N_TK_KW_DOUBLE,
	N_TK_KW_GLOBALS_COUNT,
	N_TK_KW_INT32,
	N_TK_KW_PROCEDURE,
	N_TK_KW_STRING,
	N_TK_KW_VERSION,

	N_TK_UNKNOWN = -1
} n_token_type_t;

typedef struct n_token_t n_token_t;

struct n_token_t {
	n_token_type_t type;
	char* lexeme;
};


void
n_destroy_token(n_token_t token);

n_token_t
n_get_next_token(n_stream_t* stream);

#endif
