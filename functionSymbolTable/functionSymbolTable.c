#include "functionSymbolTable.h"

static FunctionSymbolTable fst;

#define SYMBOL_TABLE_SIZE 26

typedef struct FunctionSymbolTableCDT {
    AstFunctionDeclarationNode * sym[SYMBOL_TABLE_SIZE];
} FunctionSymbolTableCDT;


void function_symbol_table_initialize() {

    fst = calloc(1, sizeof(*fst));
}

bool function_symbol_table_add(AstFunctionDeclarationNode *functionNode) {

    if(function_symbol_table_exists(functionNode->functionName))
        return false;

    fst->sym[(int)functionNode->functionName[0] - 'a'] = functionNode;

    return true;
}

AstFunctionDeclarationNode * function_symbol_table_get(char* functionName) {

    return fst->sym[(int)functionName[0] - 'a'];
}

bool function_symbol_table_exists(char* functionName) {

    return fst->sym[(int)functionName[0] - 'a'] != NULL;
}

void function_symbol_table_free() {

    for(int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        if(fst->sym[i] != NULL) 
            free_ast_tree((AstNode*)fst->sym[i]);
    }

    free(fst);
}