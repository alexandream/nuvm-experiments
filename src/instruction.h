#ifndef __NUVM__INSTRUCTION_H__
#define __NUVM__INSTRUCTION_H__

#include "util/types.h"

#define N_OP_CALL        0x00
#define N_OP_CALL_SVA    0x01
#define N_OP_GLOBAL_REF  0x02
#define N_OP_GLOBAL_SET  0x03
#define N_OP_JUMP        0x04
#define N_OP_JUMP_IF     0x05
#define N_OP_JUMP_UNLESS 0x06
#define N_OP_RETURN      0x07



typedef union NInstruction NInstruction;

union NInstruction {
	uint32_t contents;
	struct {
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
		uint8_t opcode;
	} base;
	struct {
		uint8_t arg4;
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
	} extra;
};

NInstruction
n_instruction(uint8_t opcode, uint8_t arg1, uint8_t arg2, uint8_t arg3);

void
n_decode_call(NInstruction, uint8_t* dest, uint8_t* callee, uint8_t* nargs);

void
n_decode_call_sva(NInstruction, uint8_t* dest, uint8_t* func, uint8_t* arg);

void
n_decode_global_ref(NInstruction, uint8_t* dest, uint16_t* src);

void
n_decode_global_set(NInstruction, uint16_t* dest, uint8_t* src);

void
n_decode_jump(NInstruction, int32_t* offset);

void
n_decode_jump_if(NInstruction, uint8_t* cond, int16_t* offset);

void
n_decode_jump_unless(NInstruction, uint8_t* cond, int16_t* offset);

void
n_decode_return(NInstruction, uint8_t* src);

NInstruction
n_op_call(uint8_t dest, uint8_t callee, uint8_t nargs);

NInstruction
n_op_call_sva(uint8_t dest, uint8_t func, uint8_t arg);

NInstruction
n_op_global_ref(uint8_t dest, uint16_t src);

NInstruction
n_op_global_set(uint16_t dest, uint8_t src);

NInstruction
n_op_jump(int32_t offset);

NInstruction
n_op_jump_if(uint8_t cond, int16_t offset);

NInstruction
n_op_jump_unless(uint8_t cond, int16_t offset);

NInstruction
n_op_return(uint8_t src);
#endif
