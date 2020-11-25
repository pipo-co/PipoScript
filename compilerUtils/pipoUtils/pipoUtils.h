#ifndef PIPO_UTILS_H_df37bda300314398a4386c69a646b7b10eb995a0
#define PIPO_UTILS_H_df37bda300314398a4386c69a646b7b10eb995a0

#include <stdlib.h>
#include <stdio.h>

// Function to call before exiting with errors
extern void finalize(int status);

void * emalloc(size_t size);

void * ecalloc(size_t n, size_t size);

void print_and_abort(char *message, int errorStatus);

void print_lineno_and_abort(char *message, char* filename, int lineno, int errorStatus);

#endif