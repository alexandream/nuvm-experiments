#ifndef __N__ASM__assembler_IH__
#define __N__ASM__assembler_IH__

#include "assembler.h"

typedef struct {
	char* name;
	uint32_t definition;
} NLabel;

typedef struct {
	uint8_t type;
	int64_t integer;
	uint16_t aux_integer;
	double real;
	char* text;
} NConstantDescriptor;


/* Instantiating the resizable-array template for the label pool */
#define ARRAY_TYPE_NAME NLabelArray
#define ARRAY_CONTENTS_TYPE NLabel
#define ARRAY_PREFIX nlarray
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

/* Instantiating the resizable-array template for the instructions pool */
#define ARRAY_TYPE_NAME NCodePool
#define ARRAY_CONTENTS_TYPE NInstruction*
#define ARRAY_PREFIX ncpool
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"

/* Instantiating the resizable-array template for the constants pool */
#define ARRAY_TYPE_NAME NConstantPool
#define ARRAY_CONTENTS_TYPE NConstantDescriptor
#define ARRAY_PREFIX ncopool
#define ARRAY_ELEMENTS__SKIP
#include "../common/utils/resizable-array/full.h"


struct NAssembler {
	uint8_t version[3];
	uint16_t entry_point;
	uint16_t globals_count;

	NCodePool code_pool;
	NLabelArray label_pool;
	NConstantPool constant_pool;
};

#endif
