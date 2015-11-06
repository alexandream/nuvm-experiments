#ifndef NHG_A_TOKENS
#define NHG_A_TOKENS

#include <stddef.h>

#include "istreams.h"

#define NI_TOKEN_INITIALIZER { NI_TK_UNKNOWN, NULL }

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

	NI_TK_XX_OPCODES_START,

	NI_TK_OP_GLOBAL_REF,
	NI_TK_OP_GLOBAL_SET,
	NI_TK_OP_CONST_REF,

	NI_TK_OP_MOVE_IN,
	NI_TK_OP_MOVE_OUT,

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

	NI_TK_XX_OPCODES_END,

	NI_TK_UNKNOWN = -1
} NTokenType;


typedef struct NToken NToken;

struct NToken {
	NTokenType type;
	char* lexeme;
};


void
ni_destroy_token(NToken token);

const char*
ni_get_token_name(NTokenType type);

NTokenType
ni_get_next_token(NIStream* stream, char* buffer, size_t bufsize);

NToken*
ni_token_lift(NToken token);


bool
ni_token_is_opcode(NTokenType token_type);

bool
ni_token_is_constant_keyword(NTokenType token_type);
#endif
