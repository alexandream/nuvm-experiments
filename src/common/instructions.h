#include "opcodes.h"
#include "instruction-formats.h"


int
n_encode_op_halt(NInstructionWord* words);

int
n_encode_op_nop(NInstructionWord* words);

int
n_encode_op_load_int4(NInstructionWord* words, uint8_t dest, int8_t arg);

int
n_encode_op_load_int16(NInstructionWord* words, uint8_t dest, int16_t arg);

int
n_encode_op_add(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_sub(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_mul(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_div(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_eq(NInstructionWord* words, uint8_t dest,
               uint8_t arg1, uint8_t arg2);

int
n_encode_op_neq(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_lt(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_lte(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_gt(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_gte(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_load_bool(NInstructionWord *words, uint8_t dest, uint8_t val);

int
n_encode_op_not(NInstructionWord *words, uint8_t dest, uint8_t arg);

int
n_encode_op_and(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);

int
n_encode_op_or(NInstructionWord* words, uint8_t dest,
               uint8_t arg1, uint8_t arg2);

int
n_encode_op_xor(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2);
