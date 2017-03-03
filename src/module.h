#ifndef __NUVM__MODULE_H__
#define __NUVM__MODULE_H__

#include "util/common.h"
#include "instruction.h"
#include "value.h"

typedef struct nuvm_module_t nuvm_module_t; 
typedef struct nuvm_module_builder_t nuvm_module_builder_t;

// Module

nuvm_value_t nuvm_module_load_register(nuvm_module_t* self, uint16_t index);
nuvm_value_t nuvm_module_load_entry_point(nuvm_module_t* self);

void nuvm_module_store_register(nuvm_module_t* self,
                                uint16_t index,
                                nuvm_value_t value);

nuvm_instruction_t nuvm_module_fetch(nuvm_module_t* self, uint32_t index);

void nuvm_destroy_module(nuvm_module_t* self);


// Module Builder.

nuvm_module_builder_t* nuvm_new_module_builder();

void nuvm_destroy_module_builder();

uint16_t nuvm_module_builder_push_register(nuvm_module_builder_t* builder,
                                           nuvm_value_t value);

uint32_t nuvm_module_builder_push_instruction(nuvm_module_builder_t* builder,
                                              nuvm_instruction_t inst);

nuvm_module_t* nuvm_module_builder_build(nuvm_module_builder_t* self,
                                         uint16_t entry_point);

#endif

