#ifndef __TAG_SERVICE_H__
#define __TAG_SERVICE_H__

#include "khash.h"

KHASH_MAP_INIT_STR(att, char *)

struct tag;
typedef struct tag_node {

    struct tag *tag;
    struct tag_node *next;
} tag_node_t;
struct tag_list_h {
    
    struct tag_node *first;
    struct tag_node *last;
};

typedef struct tag {
    
    char *name;
    khash_t(att) *attributes;
    char *body;
    struct tag_list_h children;
} tag_t;

// Contexto
void tag_service_init();

void tag_service_fin();

void renderTag(tag_t *t, int ind);

void renderNodeList(tag_node_t *n, int ind);

void renderAttributes(tag_t *t);

void freeTag(void * ptr);

// Runtime
tag_t *newTag();

void appendTag(tag_t *parent, tag_t *child);

int putAttribute(tag_t *tag, char *attributeName, char *attributeValue);

char *getAttribute(tag_t *tag, const char *attributeName);

#endif