#include "yaccUtils.h"
#include "lexUtils.h"


AstNode *astTree;
SymbolTable globalSt;


void yyerror(char const *s) {

	fprintf(stderr, "Error in line %d: ", yylineno);
	fprintf(stderr, "\"%s\"\n",s);

	finalize(3);
}

void initialize(void) {
    initialize_ast_node_functions();
	globalSt = symbol_table_create();
	initializeLex();
}

void finalize(int status) {

    symbol_table_free(globalSt);
    free_ast_tree(astTree);
	finalizeLex();

	exit(status);
}