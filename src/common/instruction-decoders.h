#ifndef N_C_INSTRUCTION_DECODERS_H
#define N_C_INSTRUCTION_DECODERS_H

#include "instruction-format-decoders.h"

#include "compatibility/stdint.h"


static int
n_decode_op_load_int4(NInstructionWord* words, uint8_t *dest,  int8_t *arg) {
    uint8_t opcode;
    uint8_t pad_arg;
    int result = n_decode_instruction_4_4(words, &opcode, dest, &pad_arg);
    *arg = pad_arg -8;
    return result;
}


static int
n_decode_op_load_int16(NInstructionWord* words, uint8_t *dest, int16_t *arg) {
    uint8_t opcode;
    uint16_t pad_arg;
    int result = n_decode_instruction_8_16(words, &opcode, dest, &pad_arg);
    *arg = pad_arg -16384;
    return result;
}


static int
n_decode_op_add(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


static int
n_decode_op_sub(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


static int
n_decode_op_mul(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


static int
n_decode_op_div(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


static int
n_decode_op_load_bool(NInstructionWord *words, uint8_t *dest, uint8_t *value) {
    uint8_t opcode;
    return n_decode_instruction_4_4(words, &opcode, dest, value);
}


static int
n_decode_op_not(NInstructionWord *words, uint8_t *dest, uint8_t *arg) {
    uint8_t opcode;
    return n_decode_instruction_4_4(words, &opcode, dest, arg);
}


static int
n_decode_op_and(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


int
n_decode_op_or(NInstructionWord* words, uint8_t *dest,
               uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}


int
n_decode_op_xor(NInstructionWord* words, uint8_t *dest,
                uint8_t *arg1, uint8_t *arg2) {
    uint8_t opcode;
    return n_decode_instruction_8_8_8(words, &opcode, dest, arg1, arg2);
}
#endif /* N_C_INSTRUCTION_DECODERS_H */
