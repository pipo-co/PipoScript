#include "pipoUtils.h"

#include <stdio.h>
#include <stdarg.h>


void * emalloc(size_t size) {

    void * pointer = malloc(size);

    if (!pointer) 
        print_and_abort(-1, "Error: malloc() failed!\n");

    return pointer;
}

void * ecalloc(size_t n, size_t size) {

    void * pointer = calloc(n, size);

    if (!pointer)
        print_and_abort(-1, "Error: calloc() failed!\n");
    
    return pointer;
}

void print_and_abort(int errorStatus, char *message, ...) {

    va_list args;

    va_start(args, message);

    fprintf(stderr, "\nFatal Error: ");

    vfprintf(stderr, message, args);

    putchar('\n');

    va_end(args);

    finalize(errorStatus);
}

void print_lineno_and_abort(char* filename, int lineno, int errorStatus, char *message, ...) {

    va_list args;

    va_start(args, message);

    fprintf(stderr, "\nFatal Error in File %s, Line %d: ", filename, lineno);

    vfprintf(stderr, message, args);

    putchar('\n');

    va_end(args);

    finalize(errorStatus);
}