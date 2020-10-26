#include "mm.h"
#include "tag.h"

MemoryManagerHeader memory_manager_init(NodeFreer freer){

    MemoryManagerHeader mm;
    mm.first = NULL;
    mm.last = NULL;
    mm.freer = free;

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
    memory_manager_free_node(mm->first, mm->freer);
}
void memory_manager_free_node(MemoryManagerNode *n, NodeFreer freer) {

    if(n == NULL)
        return;

    MemoryManagerNode *aux = n->next;
    freer(n->ptr);
    free(n);
    memory_manager_free_node(aux, freer);
}