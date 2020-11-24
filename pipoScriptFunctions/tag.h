#ifndef __TAG_SERVICE_H__
#define __TAG_SERVICE_H__

#include "libraries/khash.h"
#include <stdbool.h>

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

void render_tag(Tag *t, int ind);

void render_node_list(TagNode *n, int ind);

void render_attributes(Tag *t);

void free_tag(void * ptr);

// Runtime
Tag *new_tag();

void append_tag(Tag *parent, Tag *child);

bool put_attribute(Tag *tag, char *attributeName, char *attributeValue);

char *get_attribute(Tag *tag, const char *attributeName);

#endif