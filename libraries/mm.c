#include "mm.h"

MemoryManagerHeader memory_manager_init(){

    MemoryManagerHeader mm;
    mm.first = NULL;
    mm.last = NULL;

    return mm;
}

void *memory_manager_alloc(MemoryManagerHeader *mm, size_t size){

    void *ans = malloc(size);

    MemoryManagerNode *node = malloc(sizeof(*node));
    
    node->next = NULL;
    node->ptr = ans;
    
    if(mm->first == NULL)
        mm->first = node;
    else
        mm->last->next = node;

    mm->last = node;

    return ans;
}

void memory_manager_free_all(MemoryManagerHeader *mm) {
    memory_manager_free_node(mm->first);
}

void memory_manager_free_node(MemoryManagerNode *n) {

    if(n == NULL)
        return;

    MemoryManagerNode *aux = n->next;
    free(n->ptr);
    free(n);
    memory_manager_free_node(aux);
}