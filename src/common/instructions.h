#ifndef N_C_INSTRUCTIONS_H
#define N_C_INSTRUCTIONS_H

#include "compatibility/stdint.h"

#define N_MAX_INSTRUCTION_WORDS 4

typedef uint16_t NInstructionWord;


int n_encode_instruction_4_4(NInstructionWord* stream, uint8_t opcode,
		                     uint8_t arg1, uint8_t arg2);

int n_decode_instruction_4_4(NInstructionWord* stream, uint8_t* opcode,
		                     uint8_t* arg1, uint8_t* arg2);

int n_encode_instruction_8(NInstructionWord* stream, uint8_t opcode,
		                     uint8_t arg);

int n_decode_instruction_8(NInstructionWord* stream, uint8_t* opcode,
		                     uint8_t* arg);

int n_encode_instruction_8_8_8(NInstructionWord* stream, uint8_t opcode,
                               uint8_t arg1, uint8_t arg2, uint8_t arg3);

int n_decode_instruction_8_8_8(NInstructionWord* stream, uint8_t* opcode,
                               uint8_t* arg1, uint8_t* arg2, uint8_t* arg3);

int n_encode_instruction_8_16(NInstructionWord* stream, uint8_t opcode,
                              uint8_t arg1, uint16_t arg2);

int n_decode_instruction_8_16(NInstructionWord* stream, uint8_t* opcode,
                              uint8_t* arg1, uint16_t* arg2);

#endif /*N_C_INSTRUCTIONS_H*/

