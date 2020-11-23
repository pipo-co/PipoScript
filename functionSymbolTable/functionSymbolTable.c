#include "functionSymbolTable.h"
#include <string.h>

KHASH_MAP_INIT_STR(functionSymbolTable, AstFunctionDeclarationNode *)

static FunctionSymbolTable fst;

typedef struct FunctionSymbolTableCDT {
    khash_t(functionSymbolTable) *functionSymbolNodes;
} FunctionSymbolTableCDT;


FunctionSymbolTable function_symbol_table_create() {

    FunctionSymbolTable fst = calloc(1, sizeof(*fst));
    fst->functionSymbolNodes = kh_init(functionSymbolTable);

    return fst;
}

bool function_symbol_table_add(AstFunctionDeclarationNode *functionNode) {
    
    if(functionNode == NULL){
        return false;
    }

    int exist = function_symbol_table_exists(functionNode->functionName);
    int ret;

    if(exist == -1){
        khiter_t k = kh_put(functionSymbolTable, fst->functionSymbolNodes, (int)functionNode->functionName, &ret); 
        if(ret == -1){
            return false;
        }
        kh_value(fst->functionSymbolNodes, k) = functionNode;            
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
        return kh_value(fst->functionSymbolNodes, k);
    }
    return NULL;
}

int function_symbol_table_exists(char* functionName) {

     if(functionName == NULL){
        return -1;
    }

    khiter_t k;
    int flag = 1;

    for (k = kh_begin(fst->symbolNodes); flag && k != kh_end(fst->functionSymbolNodes); ++k){
        if (kh_exist(fst->functionSymbolNodes, k) && !strcmp(kh_key(fst->functionSymbolNodes, k), functionName)){
            flag = 0;
        }  
    }
        
    if(flag) {
        return -1; //si el flag esta en 1 es que no existe 
    }  
        
    return k - 1; 
}

void symbol_table_free(){

    for (khiter_t k = kh_begin(fst->functionSymbolNodes); k != kh_end(fst->functionSymbolNodes); ++k){
        if (kh_exist(fst->functionSymbolNodes, k)){
            free(kh_value(fst->functionSymbolNodes, k));
        }  
    }

    kh_destroy(functionSymbolTable, fst->functionSymbolNodes);

    free(fst);
}