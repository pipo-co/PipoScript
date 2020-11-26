#include "symbolTable.h"

#include <string.h>

#include "utils/errorHandling.h"
#include "libraries/khash.h"

KHASH_MAP_INIT_STR(symbolTable, SymbolNode *)

typedef struct SymbolTableCDT {

    khash_t(symbolTable) *symbolNodes;
    SymbolTable prev;
    SymbolTable next;

} SymbolTableCDT;

static SymbolTable activeTablesList = NULL; 

static SymbolNode * new_symbol_node(char *name, int type);


SymbolTable symbol_table_create(){
    SymbolTable st = calloc(1, sizeof(*st));
    st->symbolNodes = kh_init(symbolTable);

    if(activeTablesList == NULL) {
        st->next = NULL;
        st->prev = NULL;
    }

    else {
        st->next = activeTablesList;
        activeTablesList->prev = st;

        st->prev = NULL;
        activeTablesList = st;
    }

    return st;
}


SymbolNode * symbol_table_add(SymbolTable st, char *name, int type){

    if(name == NULL){
        return NULL;
    }

    int ret = 0;
     
    if(!symbol_node_exists(st, name)){

        SymbolNode *node = new_symbol_node(name, type);

        khiter_t k = kh_put(symbolTable, st->symbolNodes, name, &ret);
        
        if(ret == -1){
            return NULL;
        }
        
        kh_value(st->symbolNodes, k) = node;            
        
        return node;      
    }
    
    return NULL;   
}

bool symbol_node_exists(SymbolTable st, char *name){
    
    if(name == NULL){
        return false;
    }

    khiter_t k = kh_get(symbolTable, st->symbolNodes, name);

    return k != kh_end(st->symbolNodes);
}

SymbolNode * symbol_table_get(SymbolTable st, char *name){

    if(name == NULL){
        return NULL;
    }
    khiter_t k = kh_get(symbolTable, st->symbolNodes, name);

    if(k != kh_end(st->symbolNodes)){
        return kh_value(st->symbolNodes, k);
    }
    return NULL;
}

void symbol_table_free(SymbolTable st){
 
    SymbolNode *node;

    kh_foreach_value(st->symbolNodes, node, free(node));

    if(st->prev == NULL) {
        activeTablesList = st->next;
    }

    else {
        st->next->prev = st->prev;
        st->prev->next = st->next;
    }

    kh_destroy(symbolTable, st->symbolNodes);

    free(st);
}

void symbol_table_clean_up() {

    SymbolTable iter = activeTablesList;

    while(iter != NULL) {

        symbol_table_free(iter);
        iter = activeTablesList;
    }
}

static SymbolNode * new_symbol_node(char *name, int type) {

    SymbolNode *node = ecalloc(1, sizeof(*node));

    node->name = name;
    node->type = type;
    node->initialized = false;

    return node;
}