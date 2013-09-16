#ifndef __NUVM__INSTRUCTION_H__
#define __NUVM__INSTRUCTION_H__

#include "util/common.h"

#define OP_GLOBAL_REF 0x00
#define OP_GLOBAL_SET 0x01
#define OP_SVA_CALL   0x02
#define OP_RETURN     0x03

// FIXME [0001]: Assumed alignment and endianess.
typedef union nuvm_instruction_t {
	uint32_t contents;
	struct {
		uint8_t arg3;
		uint8_t arg2;
		uint8_t arg1;
		uint8_t opcode;
	}; 
} nuvm_instruction_t;


nuvm_instruction_t nuvm_op_global_ref(uint8_t local, uint16_t global);
nuvm_instruction_t nuvm_op_global_set(uint16_t global, uint8_t local);
nuvm_instruction_t nuvm_op_sva_call(uint8_t prim, uint8_t out, uint8_t arg);
nuvm_instruction_t nuvm_op_return(uint8_t result);
#endif

