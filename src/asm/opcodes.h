#ifndef __N__ASM__OPCODES__H__
#define __N__ASM__OPCODES__H__

#include "tokens.h"
#include "../common/opcodes.h"

#define NI_OPCODE_MAPPINGS \
	X( "move"      , NI_TK_OP_MOVE      ,  N_OP_MOVE       )\
	X( "global-ref", NI_TK_OP_GLOBAL_REF,  N_OP_GLOBAL_REF )\
	X( "global-set", NI_TK_OP_GLOBAL_SET,  N_OP_GLOBAL_SET )\
	X( "jump"      , NI_TK_OP_JUMP      ,  N_OP_JUMP       )\
	X( "jump-if"   , NI_TK_OP_JUMP_IF   ,  N_OP_JUMP_IF    )\
	X( "eq"        , NI_TK_OP_EQ        ,  N_OP_EQ         )\
	X( "lt"        , NI_TK_OP_LT        ,  N_OP_LT         )\
	X( "le"        , NI_TK_OP_LE        ,  N_OP_LE         )\
	X( "gt"        , NI_TK_OP_GT        ,  N_OP_GT         )\
	X( "ge"        , NI_TK_OP_GE        ,  N_OP_GE         )\
	X( "not"       , NI_TK_OP_NOT       ,  N_OP_NOT        )\
	X( "or"        , NI_TK_OP_OR        ,  N_OP_OR         )\
	X( "and"       , NI_TK_OP_AND       ,  N_OP_AND        )\
	X( "add"       , NI_TK_OP_ADD       ,  N_OP_ADD        )\
	X( "sub"       , NI_TK_OP_SUB       ,  N_OP_SUB        )\
	X( "mul"       , NI_TK_OP_MUL       ,  N_OP_MUL        )\
	X( "div"       , NI_TK_OP_DIV       ,  N_OP_DIV        )\
	X( "load-bool" , NI_TK_OP_LOAD_BOOL ,  N_OP_LOAD_BOOL  )

#endif
