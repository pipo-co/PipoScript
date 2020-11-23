#include <stdlib.h>
#include <stdio.h>
#include "tag.h"
#include "mm.h"


MemoryManagerHeader mm_tag;
MemoryManagerHeader mm_tag_node;


void tag_service_init() {
    mm_tag = memory_manager_init(freeTag);
    mm_tag_node = memory_manager_init(free);
}


void tag_service_fin() {
    memory_manager_free_all(&mm_tag);
    memory_manager_free_all(&mm_tag_node);
}

tag_t *newTag(char *name) {
    
    tag_t *t = memory_manager_alloc(&mm_tag, sizeof(*t));
    t->name = name;
    t->attributes = kh_init(att);
    t->body = NULL;
    t->children.first = NULL;
    t->children.last = NULL;
    return t;
}

void appendTag(tag_t *parent, tag_t *child) {

    tag_node_t * newNode = memory_manager_alloc(&mm_tag_node, sizeof(tag_node_t));
    newNode->tag = child;
    newNode->next = NULL;

    if (parent->children.first == NULL)
       parent->children.last = parent->children.first = newNode;
    else
       parent->children.last = parent->children.last->next = newNode; 

}

int putAttribute(tag_t *tag, char *attributeName, char *attributeValue) {
    khiter_t k;
    int ret, keyNotPresent = 1;

    for (k = kh_begin(tag->attributes); keyNotPresent && k != kh_end(tag->attributes); k++)
        if (kh_exist(tag->attributes, k) && !strcmp((char *) kh_key(tag->attributes, k), (char *) attributeName))
            keyNotPresent = 0;

    if(keyNotPresent) {
        khiter_t k = kh_put(att, tag->attributes, attributeName, &ret);  // Obtengo el puntero par la llave "attributeName"
        kh_value(tag->attributes, k) = attributeValue;  // Asigno el valor "attributeValue" a la llave anterior             
        return ret;      
    }
    else {
        kh_value(tag->attributes, k - 1) = attributeValue;
        return 0;
    }
    
    return -1;        
}

char *getAttribute(tag_t *tag, const char *attributeName) {
    if( attributeName == NULL)
        return NULL;

    khiter_t k = kh_get(att, tag->attributes, attributeName);        

    if (k == kh_end(tag->attributes))
        return NULL;

    return (char *) kh_value(tag->attributes, k);
}

void renderTag(tag_t *t, int ind) {

    // <#name# #attribute_1_name#=#attribute_1_value# #attribute_n_name#=#attribute_n_value#>
    for (size_t i = 0; i < ind; i++) putchar('\t');
    printf("<%s", t->name);
    renderAttributes(t);
    printf(">\n");

    // #body#
    if(t->body != NULL){
        for (size_t i = 0; i < ind; i++) putchar('\t');
        printf("\t%s\n", t->body);
    }

    // #child_1# #child_2# ... #child_n#
    renderNodeList(t->children.first, ind);

    // </#name#>
    for (size_t i = 0; i < ind; i++) putchar('\t');    
    printf("</%s>\n", t->name);
}

void renderNodeList(tag_node_t *n, int ind) {

    if(n == NULL)
        return;

    renderTag(n->tag, ind + 1);
    renderNodeList(n->next, ind);
}

void renderAttributes(tag_t *t){
    
    khiter_t k, end = kh_end(t->attributes);
    const char * attribute;

    for (k = kh_begin(t->attributes); k != end; ++k){
        if(kh_exist(t->attributes, k)){
            attribute = kh_key(t->attributes, k);
            printf(" %s=\"%s\"", attribute, getAttribute(t, attribute));
        }
    }
}

void freeTag(void * ptr){

    tag_t * t = (tag_t *) ptr;
    
    if(t == NULL)
        return;

    kh_destroy(att, t->attributes);
    free(ptr);
}
