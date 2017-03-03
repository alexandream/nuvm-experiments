#ifndef __NUVM__MODULE_I_H__
#define __NUVM__MODULE_I_H__

#include "module.h"

#include "nuvm.h"
#include "value.h"
#include "instruction.h"

#define ARRAY_TYPE_NAME     register_pool_t
#define ARRAY_CONTENTS_TYPE nuvm_value_t
#define ARRAY_ALLOCATOR     nuvm_alloc_unmanaged
#define ARRAY_DEALLOCATOR   nuvm_free
#include "util/array.h"

#define ARRAY_TYPE_NAME     code_segment_t
#define ARRAY_CONTENTS_TYPE nuvm_instruction_t
#define ARRAY_ALLOCATOR     nuvm_alloc_unmanaged
#define ARRAY_DEALLOCATOR   nuvm_free
#include "util/array.h"

#define STACK_TYPE_NAME     register_stack_t
#define STACK_CONTENTS_TYPE nuvm_value_t
#define STACK_ALLOCATOR     nuvm_alloc_unmanaged
#define STACK_DEALLOCATOR   nuvm_free
#include "util/resizable_stack.h"

#define STACK_TYPE_NAME     code_stack_t
#define STACK_CONTENTS_TYPE nuvm_instruction_t
#define STACK_ALLOCATOR     nuvm_alloc_unmanaged
#define STACK_DEALLOCATOR   nuvm_free
#include "util/resizable_stack.h"

struct nuvm_module_t {
	uint16_t entry_point;
	register_pool_t registers;
	code_segment_t code_segment;
};


struct nuvm_module_builder_t {
	register_stack_t registers;
	code_stack_t instructions;
};

nuvm_module_t* nuvm_new_blank_module(uint16_t num_registers,
                                     uint32_t num_instructions);

#endif
