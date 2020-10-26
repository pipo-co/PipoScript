#ifndef __MM_H__
#define __MM_H__

#include <stdlib.h>

typedef struct MemoryManagerNode{
    void *ptr;
    struct MemoryManagerNode * next;
} MemoryManagerNode;

typedef struct MemoryManagerHeader{
    struct MemoryManagerNode * first;
    struct MemoryManagerNode * last;
} MemoryManagerHeader;

MemoryManagerHeader memory_manager_init();

void *memory_manager_alloc(MemoryManagerHeader *mm, size_t size);

void memory_manager_free_all(MemoryManagerHeader *mm);

void memory_manager_free_node(MemoryManagerNode *n);

#endif