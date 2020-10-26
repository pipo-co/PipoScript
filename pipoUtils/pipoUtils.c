#include "pipoUtils.h"

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

    fprintf(stderr, "%s\n", message);
    abort();
}