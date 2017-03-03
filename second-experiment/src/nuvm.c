#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "nuvm.h"
#include "symbol-pool.h"

#include "objects/bundles.h"
#include "objects/primitives.h"
#include "objects/procedures.h"
#include "objects/strings.h"

#ifndef N_CWD_MAX_LENGTH
#define N_CWD_MAX_LENGTH 1024
#endif

char *n_cwd;

static char _cwd_storage[N_CWD_MAX_LENGTH];

void
n_init() {
	n_cwd = _cwd_storage;

	if (getcwd(_cwd_storage, N_CWD_MAX_LENGTH) == NULL) {
		fprintf(stderr, "Could not discover current working directory: %s.\n",
				strerror(errno));
		exit(1);
	}

	n_init_values();

	n_init_bundles();
	n_init_primitives();
	n_init_procedures();
	n_init_strings();
}
