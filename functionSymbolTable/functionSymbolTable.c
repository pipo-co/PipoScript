#include "functionSymbolTable.h"
#include <string.h>

KHASH_MAP_INIT_STR(functionSymbolTable, AstFunctionDeclarationNode *)

static khash_t(functionSymbolTable) *fst;

void function_symbol_table_initialize() {

    fst = kh_init(functionSymbolTable);
}

bool function_symbol_table_add(AstFunctionDeclarationNode *functionNode) {
    
    if(functionNode == NULL){
        return false;
    }

    int exist = function_symbol_table_exists(functionNode->functionName);
    int ret;

    if(exist == -1){
        khiter_t k = kh_put(functionSymbolTable, fst, (int)functionNode->functionName, &ret); 
        if(ret == -1){
            return false;
        }
        kh_value(fst, k) = functionNode;            
        return true;
    }

    return false;
}

AstFunctionDeclarationNode * function_symbol_table_get(char* functionName) {

    if( functionName == NULL){
        return NULL;
    }

    int k = function_symbol_table_exists(functionName);
    if(k != -1){
        return kh_value(fst, k);
    }
    return NULL;
}

int function_symbol_table_exists(char* functionName) {

     if(functionName == NULL){
        return -1;
    }

    khiter_t k;
    int flag = 1;

    for (k = kh_begin(fst); flag && k != kh_end(fst); ++k){
        if (kh_exist(fst, k) && !strcmp(kh_key(fst, k), functionName)){
            flag = 0;
        }  
    }
        
    if(flag) {
        return -1; //si el flag esta en 1 es que no existe 
    }  
        
    return k - 1; 
}

void function_symbol_table_free(){

    for (khiter_t k = kh_begin(fst); k != kh_end(fst); ++k){
        if (kh_exist(fst, k)){
            free(kh_value(fst, k));
        }  
    }

    kh_destroy(functionSymbolTable, fst);

    free(fst);
}