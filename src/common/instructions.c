#include "instructions.h"
#include "instruction-formats.h"

#include "opcodes.h"


int
n_encode_op_halt(NInstructionWord* words) {
    return n_encode_instruction_8(words, N_OP_HALT, 0x00);
}


int
n_encode_op_nop(NInstructionWord* words) {
    return n_encode_instruction_8(words, N_OP_NOP, 0x00);
}


int
n_encode_op_load_int4(NInstructionWord* words, uint8_t dest, int8_t arg) {
    uint8_t pad_arg = arg + 8;
    return n_encode_instruction_4_4(words, N_OP_LOAD_INT4, dest, pad_arg);
}


int
n_encode_op_load_int16(NInstructionWord* words, uint8_t dest, int16_t arg) {
    uint16_t pad_arg = arg + 16384;
    return n_encode_instruction_8_16(words, N_OP_LOAD_INT16, dest, pad_arg);
}


int
n_encode_op_add(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2) {
    return n_encode_instruction_8_8_8(words, N_OP_ADD, dest, arg1, arg2);
}


int
n_encode_op_sub(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2) {
    return n_encode_instruction_8_8_8(words, N_OP_SUB, dest, arg1, arg2);
}


int
n_encode_op_mul(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2) {
    return n_encode_instruction_8_8_8(words, N_OP_MUL, dest, arg1, arg2);
}


int
n_encode_op_div(NInstructionWord* words, uint8_t dest,
                uint8_t arg1, uint8_t arg2) {
    return n_encode_instruction_8_8_8(words, N_OP_DIV, dest, arg1, arg2);
}
