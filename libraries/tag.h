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

void tag_service_init();

void tag_service_fin();

tag_t *newTag(char *name);

int renderTag(tag_t *t, int ind);

int renderNodeList(tag_node_t *n, int ind);

void renderAttributes(tag_t *t);

void appendTag(tag_t *parent, tag_t *child);

int putAttribute(tag_t *tag, char *attributeName, char *attributeValue);

char *getAttribute(tag_t *tag, const char *attributeName);

void freeTag(void * ptr);

#endif