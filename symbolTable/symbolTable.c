#include "symbolTable.h"
#include <string.h>

KHASH_MAP_INIT_STR(symbolTable, SymbolNode *)

typedef struct SymbolTableCDT {
    khash_t(symbolTable) *symbolNodes;
} SymbolTableCDT;


static SymbolNode * new_symbol_node(char *name, int type);

SymbolTable symbol_table_create(){
    SymbolTable st = calloc(1, sizeof(*st));
    st->symbolNodes = kh_init(symbolTable);

    return st;
}


SymbolNode * symbol_table_add(SymbolTable st, char *name, int type){

    if( name == NULL){
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

    if( name == NULL){
        return NULL;
    }
    khiter_t k = kh_get(symbolTable, st->symbolNodes, name);

    if(k != kh_end(st->symbolNodes)){
        return kh_value(st->symbolNodes, k);
    }
    return NULL;
}

void symbol_table_free(SymbolTable st){
    for (khiter_t k = kh_begin(st->symbolNodes); k != kh_end(st->symbolNodes); ++k){
        if (kh_exist(st->symbolNodes, k)){
            free(kh_value(st->symbolNodes, k));
        }  
    }
    kh_destroy(symbolTable, st->symbolNodes);

    free(st);
}

static SymbolNode * new_symbol_node(char *name, int type) {

    SymbolNode *node = ecalloc(1, sizeof(*node));

    node->name = name;
    node->type = type;

    return node;
}