#ifndef __TAG_SERVICE_H__
#define __TAG_SERVICE_H__

#include "khash.h"

KHASH_MAP_INIT_STR(att, char *)

struct Tag;
typedef struct TagNode {

    struct Tag *tag;
    struct TagNode *next;

} TagNode;
typedef struct TagList {
    
    struct TagNode *first;
    struct TagNode *last;

} TagList;

typedef struct Tag {
    
    char *name;
    char *body;
    khash_t(att) *attributes;
    struct TagList children;

} Tag;

// Contexto
void tag_service_init();

void tag_service_fin();

void renderTag(Tag *t, int ind);

void renderNodeList(TagNode *n, int ind);

void renderAttributes(Tag *t);

void freeTag(void * ptr);

// Runtime
Tag *newTag();

void appendTag(Tag *parent, Tag *child);

int putAttribute(Tag *tag, char *attributeName, char *attributeValue);

char *getAttribute(Tag *tag, const char *attributeName);

#endif