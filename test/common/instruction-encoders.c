#include <stdlib.h>
#include <string.h>

#include "../test.h"

#include "common/instructions.h"
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

    ASSERT(EQ_UINT(used_words, 1));
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

    ASSERT(EQ_UINT(used_words, 1));
}


AtTest* tests[] = {
    &encode_halt_has_right_opcode,
    &encode_halt_uses_one_word,
    &encode_nop_has_right_opcode,
    &encode_nop_uses_one_word,
    NULL
};


TEST_RUNNER("Instruction-Encoders", tests, NULL, NULL, NULL, NULL)
