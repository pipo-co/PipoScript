#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stringService.h"
#include "mm.h"

#define MAX_INT_SIZE 12 //Basado en 2147483647

static MemoryManagerHeader mm;

void string_service_init() {
    mm = memory_manager_init(free);
}

void string_service_fin() {
    memory_manager_free_all(&mm);
}

char *itoa2(int i) {

    char * buffer = memory_manager_alloc(&mm, MAX_INT_SIZE);

    sprintf(buffer,"%d", i);

    return buffer;
}

char *joinStrings(char *s1, char *s2) {

    size_t size1 = strlen(s1);
    size_t size2 = strlen(s2);
    char * ans = memory_manager_alloc(&mm, size1 + size2 + 1);

    memcpy(ans, s1, size1);
    memcpy(ans + size1, s2, size2);
    ans[size1 + size2] = '\0';

    return ans;
}
