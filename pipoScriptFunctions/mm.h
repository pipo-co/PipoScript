#ifndef __MM_H__
#define __MM_H__

#include <stddef.h>

typedef void (*NodeFreer)(void *);
typedef struct MemoryManagerNode{
    void *ptr;
    struct MemoryManagerNode * next;
} MemoryManagerNode;

typedef struct MemoryManagerHeader{
    struct MemoryManagerNode * first;
    struct MemoryManagerNode * last;
    NodeFreer freer;
} MemoryManagerHeader;

MemoryManagerHeader memory_manager_init(NodeFreer freer);

void *memory_manager_alloc(MemoryManagerHeader *mm, size_t size);

void memory_manager_free_all(MemoryManagerHeader *mm);

void memory_manager_free_node(MemoryManagerNode *n, NodeFreer freer);

#endif