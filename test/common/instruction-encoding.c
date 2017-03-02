#include <stdlib.h>
#include <string.h>

#include "../test.h"

#include "common/instructions.h"
#include "common/instruction-decoders.h"
#include "common/compatibility/stdint.h"


TEST(encode_halt_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_halt(words);

    ASSERT(EQ_UINT(bytes[0], N_OP_HALT));
}


TEST(encode_halt_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_op_halt(words);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(encode_nop_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_nop(words);

    ASSERT(EQ_UINT(bytes[0], N_OP_NOP));
}


TEST(encode_nop_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    /* The opcode and arguments are irrelevant to this test. */
    int used_words = n_encode_op_nop(words);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(encode_load_int4_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_load_int4(words, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_LOAD_INT4));
}


TEST(encode_load_int4_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_load_int4(words, 0, 0);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_load_int4_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t dest;
    int8_t arg;
    int used_words = n_decode_op_load_int4(words, &dest, &arg);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_load_int4_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int8_t arg = 7;
    int8_t d_arg;
    uint8_t dest = 3;
    uint8_t d_dest;

    n_encode_op_load_int4(words, dest, arg);
    n_decode_op_load_int4(words, &d_dest, &d_arg);

    ASSERT(EQ_INT(d_arg, arg));
    ASSERT(EQ_UINT(d_dest, dest));
}


TEST(encode_load_int16_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_load_int16(words, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_LOAD_INT16));
}


TEST(encode_load_int16_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_load_int16(words, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_load_int16_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int16_t arg;
    uint8_t dest;
    int used_words = n_decode_op_load_int16(words, &dest, &arg);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_load_int16_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int16_t arg = 13422;
    int16_t d_arg;
    uint8_t dest = 5;
    uint8_t d_dest;
    n_encode_op_load_int16(words, dest, arg);
    n_decode_op_load_int16(words, &d_dest, &d_arg);

    ASSERT(EQ_INT(d_arg, arg));
    ASSERT(EQ_UINT(d_dest, dest));
}


TEST(encode_add_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_add(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_ADD));
}


TEST(encode_add_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_add(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_add_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_add(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_add_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_add(words, arg1, arg2, arg3);
    n_decode_op_add(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_sub_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_sub(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_SUB));
}


TEST(encode_sub_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_sub(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_sub_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_sub(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_sub_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_sub(words, arg1, arg2, arg3);
    n_decode_op_sub(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_mul_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_mul(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_MUL));
}


TEST(encode_mul_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_mul(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_mul_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_mul(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_mul_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_mul(words, arg1, arg2, arg3);
    n_decode_op_mul(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_div_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_div(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_DIV));
}


TEST(encode_div_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_div(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_div_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_div(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_div_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_div(words, arg1, arg2, arg3);
    n_decode_op_div(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_load_bool_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_load_bool(words, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_LOAD_BOOL));
}


TEST(encode_load_bool_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_load_bool(words, 0, 0);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_load_bool_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2;
    uint8_t used_words = n_decode_op_load_bool(words, &arg1, &arg2);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_load_bool_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =12, arg2 = 7;
    uint8_t d_arg1, d_arg2;
    n_encode_op_load_bool(words, arg1, arg2);
    n_decode_op_load_bool(words, &d_arg1, &d_arg2);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
}


TEST(encode_not_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_not(words, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_NOT));
}


TEST(encode_not_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_not(words, 0, 0);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_not_uses_one_word) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2;
    uint8_t used_words = n_decode_op_not(words, &arg1, &arg2);

    ASSERT(EQ_INT(used_words, 1));
}


TEST(decode_not_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =12, arg2 = 7;
    uint8_t d_arg1, d_arg2;
    n_encode_op_not(words, arg1, arg2);
    n_decode_op_not(words, &d_arg1, &d_arg2);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
}


TEST(encode_and_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_and(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_AND));
}


TEST(encode_and_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_and(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_and_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_and(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_and_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_and(words, arg1, arg2, arg3);
    n_decode_op_and(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_or_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_or(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_OR));
}


TEST(encode_or_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_or(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_or_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_or(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_or_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_or(words, arg1, arg2, arg3);
    n_decode_op_or(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}


TEST(encode_xor_has_right_opcode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    unsigned char* bytes = (unsigned char*) words;
    n_encode_op_xor(words, 0, 0, 0);

    ASSERT(EQ_UINT(bytes[0], N_OP_XOR));
}


TEST(encode_xor_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    int used_words = n_encode_op_xor(words, 0, 0, 0);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_xor_uses_two_words) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1, arg2, arg3;
    uint8_t used_words = n_decode_op_xor(words, &arg1, &arg2, &arg3);

    ASSERT(EQ_INT(used_words, 2));
}


TEST(decode_xor_reverts_encode) {
    NInstructionWord words[N_MAX_INSTRUCTION_WORDS];
    uint8_t arg1 =123, arg2 = 213, arg3 = 11;
    uint8_t d_arg1, d_arg2, d_arg3;
    n_encode_op_xor(words, arg1, arg2, arg3);
    n_decode_op_xor(words, &d_arg1, &d_arg2, &d_arg3);

    ASSERT(EQ_UINT(d_arg1, arg1));
    ASSERT(EQ_UINT(d_arg2, arg2));
    ASSERT(EQ_UINT(d_arg3, arg3));
}



AtTest* tests[] = {
    &encode_halt_has_right_opcode,
    &encode_halt_uses_one_word,
    &encode_nop_has_right_opcode,
    &encode_nop_uses_one_word,

    &encode_load_int4_has_right_opcode,
    &encode_load_int4_uses_one_word,
    &decode_load_int4_uses_one_word,
    &decode_load_int4_reverts_encode,

    &encode_load_int16_has_right_opcode,
    &encode_load_int16_uses_two_words,
    &decode_load_int16_uses_two_words,
    &decode_load_int16_reverts_encode,

    &encode_add_has_right_opcode,
    &encode_add_uses_two_words,
    &decode_add_uses_two_words,
    &decode_add_reverts_encode,

    &encode_sub_has_right_opcode,
    &encode_sub_uses_two_words,
    &decode_sub_uses_two_words,
    &decode_sub_reverts_encode,

    &encode_mul_has_right_opcode,
    &encode_mul_uses_two_words,
    &decode_mul_uses_two_words,
    &decode_mul_reverts_encode,

    &encode_div_has_right_opcode,
    &encode_div_uses_two_words,
    &decode_div_uses_two_words,
    &decode_div_reverts_encode,

    &encode_load_bool_has_right_opcode,
    &encode_load_bool_uses_one_word,
    &decode_load_bool_uses_one_word,
    &decode_load_bool_reverts_encode,

    &encode_not_has_right_opcode,
    &encode_not_uses_one_word,
    &decode_not_uses_one_word,
    &decode_not_reverts_encode,

    &encode_and_has_right_opcode,
    &encode_and_uses_two_words,
    &decode_and_uses_two_words,
    &decode_and_reverts_encode,

    &encode_or_has_right_opcode,
    &encode_or_uses_two_words,
    &decode_or_uses_two_words,
    &decode_or_reverts_encode,

    &encode_xor_has_right_opcode,
    &encode_xor_uses_two_words,
    &decode_xor_uses_two_words,
    &decode_xor_reverts_encode,
    NULL
};


TEST_RUNNER("Instruction-Encoding", tests, NULL, NULL, NULL, NULL)
