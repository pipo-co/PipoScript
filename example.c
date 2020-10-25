// Tag div2 = new_tag "div";
// String s = "h";
// Integer i = 1;

// while(i < 4) {

//     String name = joinString(s, intToString(i));
//     Tag aux_title = new_tag name;

//     if( i % 2 == 0)
//       set body from aux_title = call joinString("Titulo par", call intToString(i));
//     else
//       set body from aux_title = call joinString("Titulo impar", call intToString(i));

//     append_child_to div = aux_title;

//     i++;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAGS 50
#define MAX_INT_SIZE 12 //Basdao en 2147483647

typedef struct {
    char * name;
    // map attributes;
    char * body;
    tagList childs;

} tag;

typedef struct {
    tag *tag;
    node *next;

} node;

typedef struct {
    
    node *first;
    node *last;

} tagList;


int main(int argc, char const *argv[]) {

    // Tag div2 = new_tag "div";
    tag div2 = newTag("div");

    // String s = "h";
    char * s = "h";

    // Integer i = 1;
    int i = 1;

    // while(i < 4) {
    while (i < 4) {

        // String name = joinString(s, intToString(i));
        char * name = joinStrings(s, itoa2(i));
        
        // Tag aux_title = new_tag name;
        tag aux_title = newTag(name);
        
        // if( i % 2 == 0)
        if( i % 2 == 0)

            // set body from aux_title = call joinString("Titulo par", call intToString(i));
            aux_title.body = joinStrings("Titulo par", itoa2(i));
        else

            // set body from aux_title = call joinString("Titulo par", call intToString(i));
            aux_title.body = joinStrings("Titulo impar", itoa2(i));
        
        // append_child_to div = aux_title;
        appendTag(&div2, &aux_title);

        // i++;
        i++;
    }
    
    return 0;
}

tag newTag(char *name){
    tag t;
    t.name = name;
    t.childs.first = NULL;
    t.childs.last = NULL;
    return t;
}

void appendTag(tag *parent, tag *child){

    node * newNode = (node *) malloc(sizeof(node));
    newNode->tag = child;
    newNode->next = NULL;
    

    if (parent->childs.first == NULL)
       parent->childs.last = parent->childs.first = newNode;
    else
       parent->childs.last = parent->childs.last->next = newNode; 

}

char *itoa2(int i){

    char * buffer = malloc(MAX_INT_SIZE);

    sprintf(buffer,"%d",i);

    return buffer;
}

char *joinStrings(char *s1, char *s2) {

    size_t size1 = strlen(s1);
    size_t size2 = strlen(s2);
    char * ans = malloc(size1 + size2 + 1);

    memcpy(ans, s1, size1);
    memcpy(ans + size1, s2, size2);
    ans[size1 + size2] = '\0';

    return ans;
}