#include "../src/asm/asm.h"
#include "runner.h"

int main(int argc, char** argv) {
	n_init_asm();
	return run(argc, argv);
}
