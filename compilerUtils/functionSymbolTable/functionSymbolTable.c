#include "functionSymbolTable.h"

#include <string.h>

#include "libraries/khash.h"

KHASH_MAP_INIT_STR(functionSymbolTable, AstFunctionDeclarationNode *)

static khash_t(functionSymbolTable) *fst;

void function_symbol_table_initialize() {

    fst = kh_init(functionSymbolTable);
}

bool function_symbol_table_add(AstFunctionDeclarationNode *functionNode) {
    
    if(functionNode == NULL){
        return false;
    }
    
    int ret;

    if(!function_symbol_table_exists(functionNode->functionName)){

        khiter_t k = kh_put(functionSymbolTable, fst, functionNode->functionName, &ret); 
        if(ret == -1){
            return false;
        }

        kh_value(fst, k) = functionNode;            
        return true;
    }

    return false;
}

AstFunctionDeclarationNode * function_symbol_table_get(char* functionName) {

    if(functionName == NULL){
        return NULL;
    }

    khiter_t k = kh_get(functionSymbolTable, fst, functionName);

    if(k != kh_end(fst)){
        return kh_value(fst, k);
    }
    return NULL;
}

bool function_symbol_table_exists(char* functionName) {

     if(functionName == NULL){
        return false;
    }

    khiter_t k = kh_get(functionSymbolTable, fst, functionName);

    return k != kh_end(fst);
}

void function_symbol_table_free(){

    AstFunctionDeclarationNode *node;

    kh_foreach_value(fst, node, free_ast_tree((AstNode*)node));

    kh_destroy(functionSymbolTable, fst);
}