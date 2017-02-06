#ifndef N_C_ERRORS_H
#define N_C_ERRORS_H

typedef struct NErrorType NErrorType;
typedef struct NError NError;

struct NErrorType {
    const char* name;
    void (*clean_up)(const NErrorType*, NError*);
};

struct NError {
    const NErrorType* type;
    const char* message;
	const void* data;
    void (*clean_up)(const NErrorType*, NError*);
};


NError
n_error_ok(void);

void
n_construct_error_type(NErrorType* type, const char* name);

void
n_register_error_type(NErrorType* type, NError* error);

NErrorType*
n_error_type(const char* name, NError* error);

int
ni_init_errors(void);


#define n_set_error(ERROR,TYPE,MSG, DATA, CLEAN) do { \
    NError* _n_e = ERROR; \
    _n_e->type = TYPE; \
    _n_e->message = MSG; \
	_n_e->data = DATA; \
	_n_e->clean_up = CLEAN; \
} while(0)

#define n_is_ok(E) ((E)->type == NULL)

void
n_destroy_error(NError* self);

#ifdef N_TEST

typedef struct NErrorRegistry NErrorRegistry;

NErrorRegistry*
nt_create_error_registry();

void
nt_destroy_error_registry(NErrorRegistry* registry);

void
nt_register_error_type(NErrorRegistry*, NErrorType*, NError*);

NErrorType*
nt_error_type(NErrorRegistry*, const char* name, NError* error);

#endif /*N_TEST*/

#endif /*N_C_ERRORS_H*/

