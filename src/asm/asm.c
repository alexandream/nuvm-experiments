#include "common/common.h"
#include "reader.h"
#include "assembler.h"

void
n_init_asm() {
	n_init_common();
	ni_init_reader();
	ni_init_assembler();
}
