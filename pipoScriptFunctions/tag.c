#include <stdlib.h>
#include <stdio.h>
#include "tag.h"
#include "mm.h"

MemoryManagerHeader mm_tag;
MemoryManagerHeader mm_tag_node;

void tag_service_init() {
    mm_tag = memory_manager_init(free_tag);
    mm_tag_node = memory_manager_init(free);
}

void tag_service_fin() {
    memory_manager_free_all(&mm_tag);
    memory_manager_free_all(&mm_tag_node);
}

Tag *new_tag() {
    
    Tag *t = memory_manager_alloc(&mm_tag, sizeof(*t));
    t->name = NULL;
    t->attributes = kh_init(att);
    t->body = NULL;
    t->children.first = NULL;
    t->children.last = NULL;
    return t;
}

void append_tag(Tag *parent, Tag *child) {

    TagNode * newNode = memory_manager_alloc(&mm_tag_node, sizeof(TagNode));
    newNode->tag = child;
    newNode->next = NULL;

    if (parent->children.first == NULL)
       parent->children.last = parent->children.first = newNode;
    else
       parent->children.last = parent->children.last->next = newNode; 

}

bool put_attribute(Tag *tag, char *attributeName, char *attributeValue) {
    khiter_t k;
    int ret, keyNotPresent = 1;

    k = kh_get(att, tag->attributes, attributeName);

    if(k == kh_end(tag->attributes)) {
        khiter_t k = kh_put(att, tag->attributes, attributeName, &ret);  // Obtengo el puntero par la llave "attributeName"
        kh_value(tag->attributes, k) = attributeValue;  // Asigno el valor "attributeValue" a la llave anterior             
        return ret < 0;      
    }
    else {
        kh_value(tag->attributes, k) = attributeValue;
        return true;
    }
    
    return false;        
}

char *get_attribute(Tag *tag, const char *attributeName) {
    if( attributeName == NULL)
        return NULL;

    khiter_t k = kh_get(att, tag->attributes, attributeName);        

    if (k == kh_end(tag->attributes))
        return NULL;

    return (char *) kh_value(tag->attributes, k);
}

void render_tag(Tag *t, int ind) {

    // <#name# #attribute_1_name#=#attribute_1_value# #attribute_n_name#=#attribute_n_value#>
    for (size_t i = 0; i < ind; i++) putchar('\t');
    printf("<%s", t->name);
    render_attributes(t);
    printf(">\n");

    // #body#
    if(t->body != NULL){
        for (size_t i = 0; i < ind; i++) putchar('\t');
        printf("\t%s\n", t->body);
    }

    // #child_1# #child_2# ... #child_n#
    render_node_list(t->children.first, ind);

    // </#name#>
    for (size_t i = 0; i < ind; i++) putchar('\t');    
    printf("</%s>\n", t->name);
}

void render_node_list(TagNode *n, int ind) {

    if(n == NULL)
        return;

    render_tag(n->tag, ind + 1);
    render_node_list(n->next, ind);
}

void render_attributes(Tag *t){
    
    khiter_t k, end = kh_end(t->attributes);
    const char * attribute;

    for (k = kh_begin(t->attributes); k != end; ++k){
        if(kh_exist(t->attributes, k)){
            attribute = kh_key(t->attributes, k);
            printf(" %s=\"%s\"", attribute, get_attribute(t, attribute));
        }
    }
}

void free_tag(void * ptr){

    Tag * t = (Tag *) ptr;
    
    if(t == NULL)
        return;

    kh_destroy(att, t->attributes);
    free(ptr);
}
