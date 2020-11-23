#include "yaccUtils.h"


void yyerror(char const *s) {

	fprintf(stderr, "Error in line %d: ", yylineno);
	fprintf(stderr, "\"%s\"\n",s);

	finalize(3);
}

void initialize(void) {
    initialize_ast_node_functions();
}

int execute_main(void) {

	AstFunctionDeclarationNode *main = function_symbol_table_get("main");

	if(main == NULL) {
		fprintf(stderr, "Main function was not declared. Aborting");
		return 2;
	}

	if(main->returnType != INT) {
		fprintf(stderr, "Line %d: Main function was declared with invalid return type. It must be int. Aborting.", main->lineno);
		return 2;
	}

	SymbolTable st = symbol_table_create();

	int returnValue = execute_ast_tree(main->block, st);

	symbol_table_free(st);

	return returnValue;
}

void finalize(int status) {

    function_symbol_table_free();
    // symbol_table_free(globalSt);

	exit(status);
}

void register_function(AstNode *node) {
	AstFunctionDeclarationNode *functionNode = (AstFunctionDeclarationNode *) node;

	if(function_symbol_table_add(functionNode)) {
		fprintf("Error in line %d: Function %s already defined", yylineno, functionNode->functionName);
		finalize(3);
	}
}