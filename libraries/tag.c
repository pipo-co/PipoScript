#include <stdlib.h>
#include <stdio.h>
#include "tag.h"
#include "mm.h"

MemoryManagerHeader mm;

void tag_service_init() {
    mm = memory_manager_init();
}

void tag_service_fin() {
    memory_manager_free_all(&mm);
}

tag_t *newTag(char *name) {
    
    tag_t *t = memory_manager_alloc(&mm, sizeof(*t));
    t->name = name;
    t->body = NULL;
    t->children.first = NULL;
    t->children.last = NULL;
    return t;
}

void appendTag(tag_t *parent, tag_t *child) {

    tag_node_t * newNode = memory_manager_alloc(&mm,sizeof(tag_node_t));
    newNode->tag = child;
    newNode->next = NULL;

    if (parent->children.first == NULL)
       parent->children.last = parent->children.first = newNode;
    else
       parent->children.last = parent->children.last->next = newNode; 

}

//TODO decidir si los \n van o no.
int renderTag(tag_t *t, int ind) {

    int ans = 0;

    // <#name# #attribute_1_name#=#attribute_1_value# #attribute_n_name#=#attribute_n_value#>
    for (size_t i = 0; i < ind; i++) putchar('\t');
    printf("<%s", t->name);
    //Render att
    printf(">\n");

    // #body#
    if(t->body != NULL){
        for (size_t i = 0; i < ind; i++) putchar('\t');
        printf("\t%s\n", t->body);
    }

    // #child_1# #child_2# ... #child_n#
    ans = renderNodeList(t->children.first, ind);

    // </#name#>
    for (size_t i = 0; i < ind; i++) putchar('\t');    
    printf("</%s>\n", t->name);

    return ans;
}

//TODO buscar mejor nombre 
int renderNodeList(tag_node_t *n, int ind) {

    if(n == NULL)
        return 0;

    renderTag(n->tag, ind + 1);
    renderNodeList(n->next, ind);

    return 0;
}