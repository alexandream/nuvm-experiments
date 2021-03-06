#include "emitter.h"
#include "../common/opcodes.h"

static uint32_t
compute_constants_segment_size(NProgram* program);

static void
emit_constants_segment(NProgram* program, NOStream* stream, NError* error);

static void
emit_code_segment(NProgram* program, NOStream* stream, NError* error);

static void
emit_headers(NProgram* program, NOStream* stream, NError* error);

static void
emit_impl_flags(NProgram* program, NOStream* stream, NError* error);

static void
emit_instruction(NInstruction* instruction, NOStream* stream, NError* error);

static void
emit_magic_number(NProgram* program, NOStream* stream, NError* error);

/*
typedef struct {
	uint8_t major_version,
	        minor_version,
	        revision;

	uint16_t entry_point;
	uint16_t globals_count;

	NConstantDescriptor* constants;
	size_t constants_size;

	NInstruction* code;
	size_t code_size;

	bool should_free_code;
	bool should_free_constants;
} NProgram;
*/

#define RETURN_ON_ERROR(EE) do { if (!n_error_ok(EE)) return; } while(0)

void
ni_emit_program(NProgram* program, NOStream* stream, NError* error) {
	n_error_reset(error);

	emit_magic_number(program, stream, error);
	RETURN_ON_ERROR(error);

	emit_impl_flags(program, stream, error);
	RETURN_ON_ERROR(error);

	emit_headers(program, stream, error);
	RETURN_ON_ERROR(error);

	emit_constants_segment(program, stream, error);
	RETURN_ON_ERROR(error);

	emit_code_segment(program, stream, error);
	RETURN_ON_ERROR(error);
}

static uint32_t
compute_constant_size(NConstantDescriptor* constant) {
	switch (constant->type) {
		case NI_CONSTANT_STRING:
		case NI_CONSTANT_CHARACTER:
			return 0;
		case NI_CONSTANT_DOUBLE:
			return sizeof(double);
		case NI_CONSTANT_INT32:
			return sizeof(int32_t);
		case NI_CONSTANT_PROCEDURE:
			return sizeof(uint32_t) + sizeof(uint16_t);
		default:
			return 0;
	}
}
static uint32_t
compute_constants_segment_size(NProgram* program) {
	size_t i;
	size_t n_constants = program->constants_size;
	uint32_t size = 0;
	NConstantDescriptor* constant;

	for (i = 0; i < n_constants; i++) {
		constant = &program->constants[i];
		size += compute_constant_size(constant);
	}
	return size;
}

static void
emit_code_segment(NProgram* program, NOStream* stream, NError* error) {
	size_t i;
	size_t n_instructions = program->code_size;
	NInstruction* instruction;

	for(i = 0; i < n_instructions; i++) {
		instruction = &program->code[i];
		emit_instruction(instruction, stream, error);
		RETURN_ON_ERROR(error);
	}
}

static void
emit_constant(NConstantDescriptor* constant, NOStream* stream, NError* error) {
	ni_ostream_write_uint8(stream, constant->type, error);
	RETURN_ON_ERROR(error);

	switch(constant->type) {
		case NI_CONSTANT_STRING:
		case NI_CONSTANT_CHARACTER:
			ni_ostream_write_uint16(stream, constant->text_length, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_data(stream,
			                      constant->text,
			                      sizeof(char),
			                      constant->text_length,
			                      error);
			RETURN_ON_ERROR(error);
			break;
		case NI_CONSTANT_DOUBLE:
			ni_ostream_write_double(stream, constant->real, error);
			RETURN_ON_ERROR(error);
			break;
		case NI_CONSTANT_INT32:
			ni_ostream_write_int32(stream, constant->int32, error);
			RETURN_ON_ERROR(error);
			break;
		case NI_CONSTANT_PROCEDURE:
			ni_ostream_write_uint32(stream, constant->label_definition, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint16(stream, constant->uint16, error);
			RETURN_ON_ERROR(error);
			break;
	}
}

static void
emit_constants_segment(NProgram* program, NOStream* stream, NError* error) {
	size_t i;
	size_t n_constants = program->constants_size;
	NConstantDescriptor* constant;

	for (i = 0; i < n_constants; i++) {
		constant = &program->constants[i];
		emit_constant(constant, stream, error);
		RETURN_ON_ERROR(error);
	}
}

static void
emit_headers(NProgram* program, NOStream* stream, NError* error) {
	uint32_t constants_segment_size;
	ni_ostream_write_uint16(stream, program->entry_point, error);
	RETURN_ON_ERROR(error);

	ni_ostream_write_uint16(stream, program->globals_count, error);
	RETURN_ON_ERROR(error);

	constants_segment_size = compute_constants_segment_size(program);
	ni_ostream_write_uint32(stream, constants_segment_size, error);
	RETURN_ON_ERROR(error);
	/* FIXME: Size of instruction hard coded here. */
	ni_ostream_write_uint32(stream, program->code_size * 4, error);
	RETURN_ON_ERROR(error);
}


static void
emit_impl_flags(NProgram* program, NOStream* stream, NError* error) {
	uint8_t flags[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	ni_ostream_write_bytes(stream, flags, 8, error);
}


static void
emit_instruction(NInstruction* instruction, NOStream* stream, NError* error) {
#define ARG_A (instruction->arg_a)
#define ARG_B (instruction->arg_b)
#define ARG_C (instruction->arg_c)
	NOpcode opcode;
	ni_ostream_write_uint8(stream, instruction->opcode, error);
	RETURN_ON_ERROR(error);

	opcode = instruction->opcode;
	switch (opcode) {
		case N_OP_MOVE_IN:

			ni_ostream_write_uint8(stream, (uint8_t) ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_MOVE_OUT:

			ni_ostream_write_uint16(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int8(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_GLOBAL_REF:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_GLOBAL_SET:

			ni_ostream_write_uint16(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_CONST_REF:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_JUMP:
			ni_ostream_write_int24(stream,
			                       instruction->argument_label_offset,
			                       error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_JUMP_IF:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int16(stream,
			                       instruction->argument_label_offset,
			                       error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_EQ:
		case N_OP_LT:
		case N_OP_LE:
		case N_OP_GT:
		case N_OP_GE:
		case N_OP_ADD:
		case N_OP_SUB:
		case N_OP_MUL:
		case N_OP_DIV:
		case N_OP_OR:
		case N_OP_AND:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int8(stream, ARG_B, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_int8(stream, ARG_C, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_NOT:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;

		case N_OP_LOAD_BOOL:
			ni_ostream_write_uint8(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint16(stream, ARG_B, error);
			RETURN_ON_ERROR(error);
			break;
		case N_OP_RETURN:
			ni_ostream_write_uint16(stream, ARG_A, error);
			RETURN_ON_ERROR(error);

			ni_ostream_write_uint8(stream, 0, error);
			RETURN_ON_ERROR(error);
			break;
	}
#undef ARG_A
#undef ARG_B
#undef ARG_C
}

static void
emit_magic_number(NProgram* program, NOStream* stream, NError* error) {
	uint8_t major = program->major_version;
	uint8_t minor = program->minor_version;
	uint8_t revision = program->revision;
	/*               {  DEL,    N,    U,    V,    M,     ?,    ?,    ? } */
	uint8_t magic[] = { 0x7F, 0x4E, 0x55, 0x56, 0x4D, 0x00, 0x00, 0x00 };
	magic[5] = major;
	magic[6] = minor;
	magic[7] = revision;
	ni_ostream_write_bytes(stream, magic, 8, error);
}

#undef RETURN_ON_ERROR 
