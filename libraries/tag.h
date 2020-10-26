#ifndef __TAG_SERVICE_H__
#define __TAG_SERVICE_H__

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
    char * name;
    // map attributes;
    char * body;
    struct tag_list_h children;
} tag_t;

void tag_service_init();

void tag_service_fin();

tag_t *newTag(char *name);

int renderTag(tag_t *t, int ind);

int renderNodeList(tag_node_t *n, int ind);

void appendTag(tag_t *parent, tag_t *child);

#endif