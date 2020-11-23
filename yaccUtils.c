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
	function_symbol_table_initialize();
    initialize_ast_node_functions();
	initializeLex();
}

int execute_main(void) {

	AstFunctionDeclarationNode *mainNode = function_symbol_table_get("main");

	if(mainNode == NULL) {
		fprintf(stderr, "Main function was not declared. Aborting");
		return 2;
	}

	if(mainNode->returnType != INT) {
		fprintf(stderr, "Line %d: Main function was declared with invalid return type. It must be int. Aborting.", mainNode->lineno);
		return 2;
	}

	SymbolTable st = symbol_table_create();

	int returnValue = execute_ast_tree(mainNode->block, st);

	symbol_table_free(st);

	return returnValue;
}

void finalize(int status) {

    function_symbol_table_free();
	symbol_table_clean_up();
	finalizeLex();

	exit(status);
}

void register_function(AstNode *node) {
	AstFunctionDeclarationNode *functionNode = (AstFunctionDeclarationNode *) node;

	if(!function_symbol_table_add(functionNode)) {
		fprintf(stderr, "Error in line %d: Function %s already defined", yylineno, functionNode->functionName);
		finalize(3);
	}
}