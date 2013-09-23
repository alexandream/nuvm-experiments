#ifndef __NUVM__INSTRUCTION_H__
#define __NUVM__INSTRUCTION_H__

#include "util/common.h"

#define OP_GLOBAL_REF  0x00
#define OP_GLOBAL_SET  0x01
#define OP_SVA_CALL    0x02
#define OP_RETURN      0x03
#define OP_JUMP        0x04
#define OP_JUMP_IF     0x05
#define OP_JUMP_UNLESS 0x06
#define OP_CALL        0x07

// FIXME [0001]: Assumed alignment and endianess.
typedef union nuvm_instruction_t {
	uint32_t contents;
	struct {
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
		uint8_t opcode;
	};
	struct {
		uint8_t arg4;
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
	} extra;

} nuvm_instruction_t;

nuvm_instruction_t nuvm_pack_op_arguments(uint8_t arg1,
                                          uint8_t arg2,
                                          uint8_t arg3,
                                          uint8_t arg4);

nuvm_instruction_t nuvm_op_global_ref(uint8_t local, uint16_t global);
void nuvm_decode_op_global_ref(nuvm_instruction_t, uint8_t*, uint16_t*);

nuvm_instruction_t nuvm_op_global_set(uint16_t global, uint8_t local);
void nuvm_decode_op_global_set(nuvm_instruction_t, uint16_t*, uint8_t*);

nuvm_instruction_t nuvm_op_sva_call(uint8_t out, uint8_t prim, uint8_t arg);
void nuvm_decode_op_sva_call(nuvm_instruction_t, uint8_t*, uint8_t*, uint8_t*);

nuvm_instruction_t nuvm_op_return(uint8_t result);
void nuvm_decode_op_return(nuvm_instruction_t, uint8_t*);

nuvm_instruction_t nuvm_op_jump(int32_t offset);
void nuvm_decode_op_jump(nuvm_instruction_t inst, int32_t *offset);

nuvm_instruction_t nuvm_op_jump_if(uint8_t condition, int16_t offset);
void nuvm_decode_op_jump_if(nuvm_instruction_t, uint8_t*, int16_t*);

nuvm_instruction_t nuvm_op_jump_unless(uint8_t condition, int16_t offset);
void nuvm_decode_op_jump_unless(nuvm_instruction_t, uint8_t*, int16_t*);

nuvm_instruction_t nuvm_op_call(uint8_t out, uint8_t prim, uint8_t nargs);
void nuvm_decode_op_call(nuvm_instruction_t, uint8_t*, uint8_t*, uint8_t*);
#endif

