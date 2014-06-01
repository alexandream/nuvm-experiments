#include <stdlib.h>

#include "streams.h"

void
n_destroy_stream(n_stream_t* self) {

}


n_stream_t*
n_new_stream_from_path(const char* path) {
	return NULL;
}


n_stream_t*
n_new_stream_from_string(const char* string) {
	return NULL;
}


bool
n_stream_eof(n_stream_t* self) {
	return false;
}


uint32_t
n_stream_length(n_stream_t* self) {
	return 0;
}


char
n_stream_peek(n_stream_t* self, bool* end) {
	return '\0';
}


char
n_stream_read(n_stream_t* self, bool* end) {
	return '\0';
}
