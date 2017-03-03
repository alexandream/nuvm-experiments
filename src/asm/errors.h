#ifndef NHG_A_ERRORS
#define NHG_A_ERRORS

#include "../common/polyfills/p-stdint.h"

struct NReaderErrorTypes {
	uint32_t EndOfFile,
	         UnexpectedToken,
	         IncompatibleRegisterType,
	         UnimplementedOpcode,
	         RegisterOutOfRange;
};

struct NAssemblerErrorTypes {
	uint32_t EntryPointOutOfBounds,
	         ConstantsNotFound,
	         EmptyConstantsList,
	         InvalidConstantSyntax,
	         CodeNotFound,
	         InvalidCodeSyntax,
	         UnexpectedToken;
};

struct NErrorTypes {
	uint32_t BadAllocation,
	         IOError,
	         BufferTooSmall,
	         UnknownError,
	         NumberOutOfBounds;

	struct NReaderErrorTypes reader;
	struct NAssemblerErrorTypes assembler;
};


extern struct NErrorTypes ni_a_errors;

bool
n_init_asm_errors(uint8_t* error_code, char** error_text);

#endif /* NHG_A_ERRORS */
