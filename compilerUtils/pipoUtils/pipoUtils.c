#include "pipoUtils.h"

#include <stdlib.h>

void * emalloc(size_t size) {

    void * pointer = malloc(size);

    if (!pointer) 
        print_and_abort("Error: malloc() failed!\n");


    return pointer;
}

void * ecalloc(size_t n, size_t size) {

    void * pointer = calloc(n, size);

    if (!pointer)
        print_and_abort("Error: calloc() failed!\n");
    

    return pointer;
}

void print_and_abort(char *message) {

    fprintf(stderr, "\nFatal Error: %s\n", message);
    finalize(1);
}

void print_lineno_and_abort(char *message, int lineno) {

    fprintf(stderr, "\nFatal Error in Line %d: %s\n", lineno, message);
    finalize(1);
}