#include "yaccUtils.h"

#include "compilerUtils/lexUtils/lexUtils.h"
#include "pipoScriptFunctions/tag.h"
#include "pipoScriptFunctions/stringService.h"

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
	tag_service_init();
    string_service_init();
}

Tag * execute_main(void) {

	AstFunctionDeclarationNode *mainNode = function_symbol_table_get("main");


	if(mainNode == NULL) {
		fprintf(stderr, "Main function was not declared. Aborting\n");
		return NULL;
	}

	if(mainNode->returnType != TAG) {
		fprintf(stderr, "Line %d: Main function was declared with invalid return type. It must be Tag. Aborting.\n", mainNode->lineno);
		return NULL;
	}

	SymbolTable st = symbol_table_create();

	Tag * t = execute_ast_tree(mainNode->block, st);

	symbol_table_free(st);

	return t;
}

void render_final_tag(Tag * tag, FILE * out){
	if(tag == NULL){
		fprintf(stderr, "No tag was return, thus, rendered the tag will not be.\n");
		return;
	}

	render_tag(tag, INIT_IND, out);

}

void finalize(int status) {

    function_symbol_table_free();
	symbol_table_clean_up();
	finalizeLex();
	tag_service_fin();
    string_service_fin();

	exit(status);
}

void register_function(AstNode *node) {
	AstFunctionDeclarationNode *functionNode = (AstFunctionDeclarationNode *) node;

	if(!function_symbol_table_add(functionNode)) {
		fprintf(stderr, "Error in line %d: Function %s already defined", yylineno, functionNode->functionName);
		finalize(3);
	}
}