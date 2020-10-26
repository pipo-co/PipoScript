#include "symbolTable.h"

#define SYMBOL_TABLE_SIZE 26

typedef struct SymbolTableCDT {
    SymbolNode * sym[SYMBOL_TABLE_SIZE];
} SymbolTableCDT;


static SymbolNode * new_symbol_node(char *name, int type);


SymbolTable symbol_table_create() {

    SymbolTable st = calloc(1, sizeof(*st));

    return st;
}

SymbolNode * symbol_table_add(SymbolTable st, char *name, int type) {

    SymbolNode *node = new_symbol_node(name, type);

    st->sym[(int)node->name[0] - 'a'] = node;

    return node;
}

SymbolNode * symbol_table_get(SymbolTable st, char* name) {

    return st->sym[(int)name[0] - 'a'];
}

void symbol_table_free(SymbolTable st) {

    for(int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        if(st->sym[i] != NULL)
            free(st->sym[i]);
    }

    free(st);
}

static SymbolNode * new_symbol_node(char *name, int type) {

    SymbolNode *node = emalloc(sizeof(*node));

    node->name = name;
    node->type = type;

    return node;
}