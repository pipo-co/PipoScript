#include "pipoUtils.h"

#include <stdlib.h>

void * emalloc(size_t size) {

    void * pointer = malloc(size);

    if (!pointer) 
        print_and_abort("Error: malloc() failed!\n", -1);

    return pointer;
}

void * ecalloc(size_t n, size_t size) {

    void * pointer = calloc(n, size);

    if (!pointer)
        print_and_abort("Error: calloc() failed!\n", -1);
    
    return pointer;
}

void print_and_abort(char *message, int errorStatus) {

    fprintf(stderr, "\nFatal Error: %s\n", message);
    finalize(errorStatus);
}

void print_lineno_and_abort(char *message, char* filename, int lineno, int errorStatus) {

    fprintf(stderr, "\nFatal Error in File %s, Line %d: %s\n", filename, lineno, message);
    finalize(errorStatus);
}