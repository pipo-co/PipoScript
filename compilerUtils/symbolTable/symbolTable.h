#ifndef SYMBOL_TABLE_H_88bb85a02ebb07e487ff1027e1e7390616b9f485
#define SYMBOL_TABLE_H_88bb85a02ebb07e487ff1027e1e7390616b9f485

#include <stdbool.h>
#include "pipoScriptFunctions/tag.h"

typedef struct SymbolTableCDT * SymbolTable;

typedef union SymbolValue {

  int intValue;

  char *stringValue;

  Tag *tagValue;

} SymbolValue;

typedef struct SymbolNode {
  char * name;

  int type;

  SymbolValue value;
} SymbolNode;

SymbolTable symbol_table_create();

SymbolNode * symbol_table_add(SymbolTable st, char *name, int type);

SymbolNode * symbol_table_get(SymbolTable st, char* name);

bool symbol_node_exists(SymbolTable st, char *name);

void symbol_table_free(SymbolTable st);

void symbol_table_clean_up(void);

#endif