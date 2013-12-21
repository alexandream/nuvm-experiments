#include "nuvm.h"

#include "objects/primitives.h"
#include "objects/procedures.h"
#include "objects/strings.h"
#include "objects/symbols.h"

void
n_init() {
	n_init_values();
	n_init_primitives();
	n_init_procedures();
	n_init_strings();
	n_init_symbols();
}
