#ifndef __N__ASM__TOKENIZER__H__
#define __N__ASM__TOKENIZER__H__

#include "streams.h"

typedef enum {
	N_TK_EOF = 0,
	N_TK_TOO_BIG,

	N_TK_DEC_INTEGER,
	N_TK_HEX_INTEGER,
	N_TK_REAL,
	N_TK_UNRECOGNIZED_OPCODE,
	N_TK_LABEL,
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

	N_TK_UNRECOGNIZED_KW,

	N_TK_OP_GLOBAL_REF,
	N_TK_OP_GLOBAL_SET,
	N_TK_OP_MOVE,

	N_TK_OP_JUMP,
	N_TK_OP_JUMP_IF,

	N_TK_OP_EQ,
	N_TK_OP_GE,
	N_TK_OP_GT,
	N_TK_OP_LE,
	N_TK_OP_LT,

	N_TK_OP_NOT,

	N_TK_OP_AND,
	N_TK_OP_OR,

	N_TK_OP_ADD,
	N_TK_OP_DIV,
	N_TK_OP_MUL,
	N_TK_OP_SUB,

	N_TK_OP_LOAD_BOOL,

	N_TK_OP_RETURN,

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
