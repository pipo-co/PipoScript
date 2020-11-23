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

// SymbolTable symbol_table_create() {

//     SymbolTable st = calloc(1, sizeof(*st));

//     return st;
// }

SymbolNode * symbol_table_add(SymbolTable st, char *name, int type){

    if( name == NULL){
        return NULL;
    }

    SymbolNode *node = new_symbol_node(name, type);
    int ret = 0;

    int exist = 0;
    exist = symbol_node_exists(st, name);
    if( exist == -1){
        khiter_t k = kh_put(symbolTable, st->symbolNodes, name, &ret); 
        kh_value(st->symbolNodes, k) = node;            
        return node;      
    }
    
    return NULL;   
}

int symbol_node_exists(SymbolTable st, char *name){
    
    if(name == NULL){
        return -1;
    }

    khiter_t k;
    int flag = 1;

    for (k = kh_begin(st->symbolNodes); flag && k != kh_end(st->symbolNodes); ++k){
        if (kh_exist(st->symbolNodes, k) && !strcmp(kh_key(st->symbolNodes, k), name)){
            flag = 0;
        }  
    }
        
    if(flag) {
        return -1; //si el flag esta en 1 es que no existe 
    }  
        
    return k - 1;   //si existe retorno la posicion en el hash
    
}
// SymbolNode * symbol_table_add(SymbolTable st, char *name, int type) {

//     SymbolNode *node = new_symbol_node(name, type);

//     st->sym[(int)node->name[0] - 'a'] = node;

//     return node;
// }

SymbolNode * symbol_table_get(SymbolTable st, char *name){

    if( name == NULL){
        return NULL;
    }

    int k = symbol_node_exists(st, name);
    if(k != -1){
        return kh_value(st->symbolNodes, k);
    }
    return NULL;
}

// SymbolNode * symbol_table_get(SymbolTable st, char* name) {

//     return st->sym[(int)name[0] - 'a'];
// }

void symbol_table_free(SymbolTable st){
    for (khiter_t k = kh_begin(st->symbolNodes); k != kh_end(st->symbolNodes); ++k){
        if (kh_exist(st->symbolNodes, k)){
            free(kh_value(st->symbolNodes, k));
        }  
    }
    kh_destroy(symbolTable, st->symbolNodes);

    free(st);
}

// void symbol_table_free(SymbolTable st) {

//     for(int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
//         if(st->sym[i] != NULL)
//             free(st->sym[i]);
//     }

//     free(st);
// }

static SymbolNode * new_symbol_node(char *name, int type) {

    SymbolNode *node = ecalloc(1, sizeof(*node));

    node->name = name;
    node->type = type;

    return node;
}