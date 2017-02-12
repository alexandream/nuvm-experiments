#include <stdlib.h>
#include <string.h>

#include "../test.h"

#include "common/instructions.h"
#include "common/compatibility/stdint.h"


typedef struct Instruction Instruction;

struct Instruction {
    uint8_t opcode;
    unsigned int arg1;
    unsigned int arg2;
    unsigned int arg3;
};



TEST(encode_4_4_is_big_endian) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t bytes[2] = { 0, 0 };

    n_encode_instruction_4_4(words, 0x12, 0x3, 0x4);
    memcpy(bytes, words, 2);

    ASSERT(EQ_UINT(bytes[0], 0x12));
}


TEST(encode_4_4_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_instruction_4_4(words, 123, 12, 3);

    ASSERT(EQ_UINT(used_words, 1));
}


TEST(decode_4_4_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t opcode, arg1, arg2;
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_decode_instruction_4_4(words, &opcode, &arg1, &arg2);

    ASSERT(EQ_UINT(used_words, 1));
}


#define INSTR_4_4(OP, A1, A2) { OP, A1, A2, 0 }
static Instruction array_4_4[] = {
    INSTR_4_4(0x00, 0x0, 0x0),
    INSTR_4_4(0x00, 0xF, 0xF),
    INSTR_4_4(0xFF, 0x0, 0xF)
};
AtArrayIterator iter_4_4 = at_static_array_iterator(array_4_4);

DD_TEST(encode_decode_4_4_preserves_data, iter_4_4, Instruction, instr) {
    uint8_t opcode = instr->opcode;
    unsigned int arg1 = instr->arg1;
    unsigned int arg2 = instr->arg2;
    uint8_t d_opcode;
    uint8_t d_arg1;
    uint8_t d_arg2;
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];

    n_encode_instruction_4_4(words, opcode, arg1, arg2);
    n_decode_instruction_4_4(words, &d_opcode, &d_arg1, &d_arg2);

    ASSERT(EQ_UINT(opcode, d_opcode));
    ASSERT(EQ_UINT(arg1, d_arg1));
    ASSERT(EQ_UINT(arg2, d_arg2));
}


TEST(encode_8_is_big_endian) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t bytes[2] = { 0, 0 };

    n_encode_instruction_8(words, 0x12, 0x34);
    memcpy(bytes, words, 2);

    ASSERT(EQ_UINT(bytes[0], 0x12));
    ASSERT(EQ_UINT(bytes[1], 0x34));
}


TEST(encode_8_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_instruction_8(words, 123, 123);

    ASSERT(EQ_UINT(used_words, 1));
}


TEST(decode_8_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t opcode, arg;
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_decode_instruction_8(words, &opcode, &arg);

    ASSERT(EQ_UINT(used_words, 1));
}


#define INSTR_8(OP, A1) { OP, A1, 0, 0 }
static Instruction array_8[] = {
    INSTR_8(0xAA, 0x12),
    INSTR_8(0x55, 0x48),
    INSTR_8(0xFF, 0x18)
};
AtArrayIterator iter_8 = at_static_array_iterator(array_8);

DD_TEST(encode_decode_8_preserves_data, iter_8, Instruction, instr) {
    uint8_t opcode = instr->opcode;
    unsigned int arg = instr->arg1;
    uint8_t d_opcode;
    uint8_t d_arg;
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];

    n_encode_instruction_8(words, opcode, arg);
    n_decode_instruction_8(words, &d_opcode, &d_arg);

    ASSERT(EQ_UINT(d_opcode, opcode));
    ASSERT(EQ_UINT(d_arg, arg));
}


TEST(encode_8_8_8_is_big_endian) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t bytes[4] = { 0, 0, 0, 0 };

    n_encode_instruction_8_8_8(words, 0x12, 0x34, 0x56, 0x78);
    memcpy(bytes, words, 4);

    ASSERT(EQ_UINT(bytes[0], 0x12));
    ASSERT(EQ_UINT(bytes[1], 0x34));
    ASSERT(EQ_UINT(bytes[2], 0x56));
    ASSERT(EQ_UINT(bytes[3], 0x78));
}


TEST(encode_8_8_8_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_instruction_8_8_8(words, 123, 123, 123, 123);

    ASSERT(EQ_UINT(used_words, 2));
}


TEST(decode_8_8_8_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t opcode, a1, a2, a3;
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_decode_instruction_8_8_8(words, &opcode, &a1, &a2, &a3);

    ASSERT(EQ_UINT(used_words, 2));
}


#define INSTR_8_8_8(OP, A1, A2, A3) { OP, A1, A2, A3 }
static Instruction array_8_8_8[] = {
    INSTR_8_8_8(0xAA, 0x12, 0x34, 0x56),
    INSTR_8_8_8(0x55, 0x48, 0xDE, 0xAD),
    INSTR_8_8_8(0xFF, 0x18, 0xBE, 0xEF)
};
AtArrayIterator iter_8_8_8 = at_static_array_iterator(array_8_8_8);

DD_TEST(encode_decode_8_8_8_preserves_data, iter_8_8_8, Instruction, instr) {
    uint8_t opcode = instr->opcode;
    unsigned int arg1 = instr->arg1;
    unsigned int arg2 = instr->arg2;
    unsigned int arg3 = instr->arg3;
    uint8_t d_opcode;
    uint8_t d_arg1;
    uint8_t d_arg2;
    uint8_t d_arg3;
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];

    n_encode_instruction_8_8_8(words, opcode, arg1, arg2, arg3);
    n_decode_instruction_8_8_8(words, &d_opcode, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_opcode, opcode));
    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}



TEST(encode_8_16_is_big_endian) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t bytes[4] = { 0, 0, 0, 0 };

    n_encode_instruction_8_16(words, 0x12, 0x34, 0x5678);
    memcpy(bytes, words, 4);

    ASSERT(EQ_UINT(bytes[0], 0x12));
    ASSERT(EQ_UINT(bytes[1], 0x34));
    ASSERT(EQ_UINT(bytes[2], 0x56));
    ASSERT(EQ_UINT(bytes[3], 0x78));
}


TEST(encode_8_16_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_instruction_8_16(words, 123, 123, 23123);

    ASSERT(EQ_UINT(used_words, 2));
}


TEST(decode_8_16_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t opcode, a1;
    uint16_t a2;
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_decode_instruction_8_16(words, &opcode, &a1, &a2);

    ASSERT(EQ_UINT(used_words, 2));
}


#define INSTR_8_16(OP, A1, A2) { OP, A1, A2, 0 }
static Instruction array_8_16[] = {
    INSTR_8_16(0xAA, 0x12, 0x3456),
    INSTR_8_16(0x55, 0x48, 0xDEAD),
    INSTR_8_16(0xFF, 0x18, 0xBEEF)
};
AtArrayIterator iter_8_16 = at_static_array_iterator(array_8_16);

DD_TEST(encode_decode_8_16_preserves_data, iter_8_16, Instruction, instr) {
    uint8_t opcode = instr->opcode;
    unsigned int arg1 = instr->arg1;
    unsigned int arg2 = instr->arg2;
    uint8_t d_opcode;
    uint8_t d_arg1;
    uint16_t d_arg2;
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];

    n_encode_instruction_8_16(words, opcode, arg1, arg2);
    n_decode_instruction_8_16(words, &d_opcode, &d_arg1, &d_arg2);

    ASSERT(EQ_UINT(d_opcode, opcode));
    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
}
 

AtTest* tests[] = {
    &encode_4_4_is_big_endian,
    &encode_4_4_uses_one_word,
    &decode_4_4_uses_one_word,
    &encode_decode_4_4_preserves_data,
    &encode_8_is_big_endian,
    &encode_8_uses_one_word,
    &decode_8_uses_one_word,
    &encode_decode_8_preserves_data,
    &encode_8_8_8_is_big_endian,
    &encode_8_8_8_uses_two_words,
    &decode_8_8_8_uses_two_words,
    &encode_decode_8_8_8_preserves_data,
    &encode_8_16_is_big_endian,
    &encode_8_16_uses_two_words,
    &decode_8_16_uses_two_words,
    &encode_decode_8_16_preserves_data,
    NULL
};


TEST_RUNNER("Instructions", tests, NULL, NULL, NULL, NULL)
