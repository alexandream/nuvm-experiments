#ifndef N_C_OPCODES_H
#define N_C_OPCODES_H

#define N_OP_NOP        0x00
#define N_OP_HALT       0x01
#define N_OP_LOAD_INT4  0x02
#define N_OP_LOAD_INT16 0x03
#define N_OP_ADD        0x04
#define N_OP_SUB        0x05
#define N_OP_MUL        0x06
#define N_OP_DIV        0x07
#define N_OP_EQ         0x0E
#define N_OP_NEQ        0x0F
#define N_OP_LT         0x10
#define N_OP_LTE        0x11
#define N_OP_GT         0x12
#define N_OP_GTE        0x13
#define N_OP_LOAD_BOOL  0x08
#define N_OP_NOT        0x09
#define N_OP_AND        0x0A
#define N_OP_OR         0x0B
#define N_OP_XOR        0x0C
#endif /* N_C_OPCODES_H */
