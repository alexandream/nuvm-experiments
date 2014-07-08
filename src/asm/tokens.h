#ifndef __N__ASM__TOKENS__H__
#define __N__ASM__TOKENS__H__

#include <stddef.h>

#include "streams.h"

typedef enum {
	NI_TK_EOF = 0,
	NI_TK_TOO_BIG,

	NI_TK_DEC_INTEGER,
	NI_TK_HEX_INTEGER,
	NI_TK_REAL,
	NI_TK_UNRECOGNIZED_OPCODE,
	NI_TK_LABEL,
	NI_TK_LABEL_DEF,
	NI_TK_REGISTER,
	NI_TK_STRING,

	NI_TK_KW_CHARACTER,
	NI_TK_KW_CODE,
	NI_TK_KW_CONSTANTS,
	NI_TK_KW_DOUBLE,
	NI_TK_KW_ENTRY_POINT,
	NI_TK_KW_GLOBALS_COUNT,
	NI_TK_KW_INT32,
	NI_TK_KW_PROCEDURE,
	NI_TK_KW_STRING,
	NI_TK_KW_VERSION,

	NI_TK_UNRECOGNIZED_KW,

	NI_TK_OP_GLOBAL_REF,
	NI_TK_OP_GLOBAL_SET,
	NI_TK_OP_MOVE,

	NI_TK_OP_JUMP,
	NI_TK_OP_JUMP_IF,

	NI_TK_OP_EQ,
	NI_TK_OP_GE,
	NI_TK_OP_GT,
	NI_TK_OP_LE,
	NI_TK_OP_LT,

	NI_TK_OP_NOT,

	NI_TK_OP_AND,
	NI_TK_OP_OR,

	NI_TK_OP_ADD,
	NI_TK_OP_DIV,
	NI_TK_OP_MUL,
	NI_TK_OP_SUB,

	NI_TK_OP_LOAD_BOOL,

	NI_TK_OP_RETURN,

	NI_TK_UNKNOWN = -1
} ni_token_type_t;

typedef struct ni_token_t ni_token_t;

struct ni_token_t {
	ni_token_type_t type;
	char* lexeme;
};


void
ni_destroy_token(ni_token_t token);

const char*
ni_get_token_name(ni_token_type_t type);

ni_token_type_t
ni_get_next_token(ni_stream_t* stream, char* buffer, size_t bufsize);

#endif
