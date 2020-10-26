#include "pipoUtils.h"

void * emalloc(size_t size) {

    void * pointer = malloc(size);

    if (!pointer) {

        printf("Error: malloc(%u) failed!\n", size);
        abort();
    }

    return pointer;
}

void * ecalloc(size_t n, size_t size) {

    void * pointer = calloc(n, size);

    if (!pointer) {

        printf("Error: calloc(%u) failed!\n", size);
        abort();
    }

    return pointer;
}