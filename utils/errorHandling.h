#ifndef ERROR_HANDLING_H_df37bda300314398a4386c69a646b7b10eb995a0
#define ERROR_HANDLING_H_df37bda300314398a4386c69a646b7b10eb995a0

#include <stdlib.h>

// Function to call before exiting with errors
extern void finalize(int status);

void * emalloc(size_t size);

void * ecalloc(size_t n, size_t size);

void print_and_abort(int errorStatus, char *message, ...);

void print_lineno_and_abort(char* filename, int lineno, int errorStatus, char *message, ...);

#endif