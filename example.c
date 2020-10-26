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

#include "tag.h"
#include "stringService.h"

#define MAX_TAGS 50

int main(int argc, char const *argv[]) {

    tag_service_init();
    string_service_init();

    // Tag div2 = new_tag "div";
    tag_t *div = newTag("div");

    // String s = "h";
    char * s = "h";

    // Integer i = 1;
    int i = 1;
    
    // while(i < 4) {
    while (i < 5) {

        // String name = joinString(s, intToString(i));
        char * name = joinStrings(s, itoa2(i));
        
        // Tag aux_title = new_tag name;
        tag_t *aux_title = newTag(name);
        
        // if( i % 2 == 0)
        if( i % 2 == 0){


            // set body from aux_title = call joinString("Titulo par", call intToString(i));
            aux_title->body = joinStrings("Titulo par", itoa2(i));
        }
        else{


            // set body from aux_title = call joinString("Titulo par", call intToString(i));
            aux_title->body = joinStrings("Titulo impar", itoa2(i));
        }
        
        // append_child_to div = aux_title;
        appendTag(div, aux_title);

        // i++;
        i++;
    }
    
    int ans = renderTag(div, 0);

    tag_service_fin();
    string_service_fin();
    return ans;
}