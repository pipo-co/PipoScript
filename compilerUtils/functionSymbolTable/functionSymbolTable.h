#ifndef FUNCTION_SYMBOL_TABLE_H_88bb85a02ebb07e487ff1027e1e7390616b9f485
#define FUNCTION_SYMBOL_TABLE_H_88bb85a02ebb07e487ff1027e1e7390616b9f485

#include "compilerUtils/AST/astNodes.h"

#include <stdbool.h>

typedef struct FunctionSymbolTableCDT * FunctionSymbolTable;

void function_symbol_table_initialize();

bool function_symbol_table_add(AstFunctionDeclarationNode *functionNode);

AstFunctionDeclarationNode * function_symbol_table_get(char* functionName);

bool function_symbol_table_exists(char* functionName);

void function_symbol_table_free();

#endif