#ifndef NHG_C_OPCODES
#define NHG_C_OPCODES

#include "polyfills/p-stdint.h"

typedef enum NOpcode {
	N_OP_GLOBAL_REF,
	N_OP_GLOBAL_SET,
	N_OP_MOVE,

	N_OP_JUMP,
	N_OP_JUMP_IF,

	N_OP_EQ,
	N_OP_GE,
	N_OP_GT,
	N_OP_LE,
	N_OP_LT,

	N_OP_NOT,

	N_OP_AND,
	N_OP_OR,

	N_OP_ADD,
	N_OP_DIV,
	N_OP_MUL,
	N_OP_SUB,

	N_OP_LOAD_BOOL,

	N_OP_RETURN
} NOpcode;

#endif
