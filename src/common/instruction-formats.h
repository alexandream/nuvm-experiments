#ifndef N_C_INSTRUCTIONS_H
#define N_C_INSTRUCTIONS_H

#include "compatibility/stdint.h"

#define N_MAX_INSTRUCTION_WORDS 4

typedef uint16_t NInstructionWord;


int
n_encode_instruction_4_4(NInstructionWord* stream, uint8_t opcode,
         uint8_t arg1, uint8_t arg2);

int
n_encode_instruction_8(NInstructionWord* stream, uint8_t opcode,
                       uint8_t arg);

int
n_encode_instruction_8_8_8(NInstructionWord* stream, uint8_t opcode,
                           uint8_t arg1, uint8_t arg2, uint8_t arg3);

int
n_encode_instruction_8_16(NInstructionWord* stream, uint8_t opcode,
                          uint8_t arg1, uint16_t arg2);


#ifndef N_INSTRUCTIONS_IGNORE_DECODE
static int
n_decode_instruction_4_4(NInstructionWord* stream, uint8_t* opcode,
                         uint8_t* arg1, uint8_t* arg2) {
	unsigned char* bytes = (unsigned char*) stream;
	*opcode = bytes[0];
	*arg1 = (bytes[1] >> 4) & 0x0F;
	*arg2 = bytes[1] & 0x0F;
    return 1;
}


static int
n_decode_instruction_8(NInstructionWord* stream, uint8_t* opcode,
                       uint8_t* arg) {
	unsigned char* bytes = (unsigned char*) stream;
	*opcode = bytes[0];
	*arg = bytes[1];
    return 1;
}


static int
n_decode_instruction_8_8_8(NInstructionWord* stream, uint8_t* opcode,
                           uint8_t* arg1, uint8_t* arg2, uint8_t* arg3) {
	unsigned char* bytes = (unsigned char*) stream;
	*opcode = bytes[0];
	*arg1 = bytes[1];
	*arg2 = bytes[2];
	*arg3 = bytes[3];
    return 2;
}


static int
n_decode_instruction_8_16(NInstructionWord* stream, uint8_t* opcode,
                          uint8_t* arg1, uint16_t* arg2) {
	unsigned char* bytes = (unsigned char*) stream;
	unsigned char* arg2_bytes = (unsigned char*) arg2;
	*opcode = bytes[0];
	*arg1 = bytes[1];
	arg2_bytes[0] = bytes[3];
	arg2_bytes[1] = bytes[2];
    return 2;
}
#endif /*N_INSTRUCTIONS_IGNORE_DECODE */
#endif /*N_C_INSTRUCTIONS_H*/

