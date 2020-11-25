#include "yaccUtils.h"

#include "compilerUtils/lexUtils/lexUtils.h"
#include "pipoScriptFunctions/tag.h"
#include "pipoScriptFunctions/stringService.h"

extern FILE *yyin;
void yyrestart(FILE *input_file);

void yyerror(char const *s) {

	fprintf(stderr, "Error in File %s, Line %d: %s\n", args.inputFiles.currentFilename, yylineno, s);
	finalize(3);
}

FILE * initialize(void) {

	// Open Output File [required]
	FILE *output = fopen(args.outputFileName, "w");

	if(output == NULL) {

		perror("Error opening output file");
		fprintf(stderr, "Without output file %s, excecution cannot continue", args.outputFileName);

		input_file_handler_finalize(&args.inputFiles);
		exit(2);
	}

	// Initialize Function Symbol Table 
	function_symbol_table_initialize();

	// Initialize Ast Functions
    initialize_ast_node_functions();

	// Initialize Lexer Utils
	initializeLex();

	// Initialize Tags Library
	tag_service_init();

	// Initialize Strings Handler Library
    string_service_init();

	return output;
}

void parse_input_files(void) {

	FILE *currentFile;

	while(currentFile = input_file_handler_dequeue(&args.inputFiles), currentFile != NULL) {

		yyin = currentFile;

		yyrestart(yyin);
	
		yyparse();
	}

	// Free And Close All Remaining Input Files
	input_file_handler_finalize(&args.inputFiles);
}

Tag * execute_main(void) {

	AstFunctionDeclarationNode *mainNode = function_symbol_table_get("main");

	if(mainNode == NULL)
		print_and_abort("Main function was not declared. Aborting\n", 4);

	if(mainNode->returnType != TAG)
		print_lineno_and_abort("File %s, Line %d: Main function was declared with invalid return type. It must be Tag. Aborting.\n", mainNode->filename, mainNode->lineno, 4);

	SymbolTable st = symbol_table_create();

	Tag * t = execute_ast_tree(mainNode->block, st);

	symbol_table_free(st);

	return t;
}

int render_main_tag(Tag * tag, FILE * out){

	if(tag == NULL){
		fprintf(stderr, "No tag was return in main function. No tag will be rendered\n");
		return 5;
	}

	render_tag(tag, INIT_IND, out);

	return 0;
}

void finalize(int status) {

	// Free Function Table
    function_symbol_table_free();

	// Free All Remaining Symbol Tables
	symbol_table_clean_up();

	// Finalize Lex Utils
	finalizeLex();

	// Finalize Tags Library
	tag_service_fin();

	// Finalize Strings Handlers Library
    string_service_fin();

	exit(status);
}

void register_function(AstNode *node) {
	AstFunctionDeclarationNode *functionNode = (AstFunctionDeclarationNode *) node;

	if(!function_symbol_table_add(functionNode)) {
		fprintf(stderr, "Error in File %s, Line %d: Function %s already defined\n", args.inputFiles.currentFilename, yylineno, functionNode->functionName);
		free(functionNode);
		finalize(3);
	}
}