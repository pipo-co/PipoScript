#include <stdlib.h>
#include <stdio.h>
#include "tag.h"
#include "mm.h"


MemoryManagerHeader mm;


void tag_service_init() {
    mm = memory_manager_init(freeTag);
}


void tag_service_fin() {
    memory_manager_free_all(&mm);
}

tag_t *newTag(char *name) {
    
    tag_t *t = memory_manager_alloc(&mm, sizeof(*t));
    t->name = name;
    t->attributes = kh_init(att);
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

int putAttribute(tag_t *tag, char *attributeName, char *attributeValue){
    int ret;
    khiter_t k = kh_put(att, tag->attributes, attributeName, &ret);  // Obtengo el puntero par la llave "attributeName"
    kh_value(tag->attributes, k) = attributeValue;  // Asigno el valor "attributeValue" a la llave anterior          
    return ret;              
}

char *getAttribute(tag_t *tag, const char *attributeName){
    if( attributeName == NULL)
        return NULL;

    khiter_t k = kh_get(att, tag->attributes, attributeName);        
    int is_missing = (k == kh_end(tag->attributes));
    if (is_missing)
        return NULL;

    return (char *) kh_value(tag->attributes, k);
}

//TODO decidir si los \n van o no.
int renderTag(tag_t *t, int ind) {

    int ans = 0;

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

void renderAttributes(tag_t *t){
    khiter_t k, end = kh_end(t->attributes);
    const char * attribute;


    for (k = kh_begin(t->attributes); k != end; ++k){
        attribute = kh_key(t->attributes, k);
        //Por alguna razon se asigna a cualquier valor dentro del hash y arranca en 4 por lo que todos los del medio quedan en null
        //habria que probar usar el foreach de khash
        if(attribute != NULL)
            printf(" %s=\"%s\"", attribute, getAttribute(t, attribute));
    }
}

void freeTag(void * ptr){

    tag_t * t = (tag_t *) ptr;

    kh_destroy(att, t->attributes);
    free(ptr);
}
