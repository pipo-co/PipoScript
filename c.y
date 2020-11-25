%{
	#include "compilerUtils/yaccUtils/yaccUtils.h"
%}

%union {

	int operation;

	int intValue;

	char *stringValue;

	char *symbol;

	AstFunctionArgList *functionArgList;

	AstNode *astNode;
}

%define parse.error verbose

%token <operation> FIRST

%token <symbol> ID

%token <intValue> INT_LITERAL
%token <stringValue> STRING_LITERAL

%token <operation> INT VOID STRING TAG
%token <operation> IF ELSE WHILE DO FOR RETURN
%token <operation> SET GET NEW NAME BODY ATTRIBUTE FROM APPEND_CHILD

%right '='
%left CONCAT
%left CMP
%left OR AND
%left '>' '<' LE GE EQ NE
%left '+' '-'
%left '*' '/' '%'
%right '!' STR LEN
%left INC DEC
%left ')'
%right '('
%nonassoc UMINUS

%type <operation> AND OR '=' '>' '<' LE GE EQ NE '+' '-' '*' '/' '%' '!' INC DEC ')' '(' UMINUS
%type <operation> CONCAT CMP LEN STR
%type <operation> SET_PROPERTY SET_NAMED_PROPERTY GET_PROPERTY GET_NAMED_PROPERTY

%nonassoc IFX
%nonassoc ELSE
%nonassoc STATEMENT_LIST_CONST FUNCTION_DECLARATION_CONST FUNCTION_CALL_CONST FUNCTION_DEFINITION_LIST_CONST
%nonassoc SET_PROPERTY_CONST SET_NAMED_PROPERTY_CONST GET_PROPERTY_CONST GET_NAMED_PROPERTY_CONST INT_CAST_CONST

%type <astNode> STATEMENT_LIST STATEMENT BLOCK if_statement iteration_statement return_statement declaration_statement
%type <astNode> assignment_statement ASSIGNMENT DO_ASSIGNMENT ARITH NUM NON_ID_NUM VALUE FUNCTION_CALL function_call_statement
%type <astNode> STRING_ARITH STRING_VALUE NON_ID_STRING_VALUE

%type <functionArgList> FUNC_ARG_LIST FUNC_DECLARATION_ARG_LIST

%type <operation> ID_TYPE FUNCTION_RETURN_TYPE

%token DECLARATION

%token LAST

%start P

%%

P       
	: FUNCTION_DEFINITION_LIST	{ 
									if(yychar == YYEOF)
										printf("Input file %s parsed successfully\n", args.inputFiles.currentFilename);
								}
	;

FUNCTION_DEFINITION_LIST
	: FUNCTION_DEFINITION_LIST FUNCTION_DEFINITION
	|
	;

STATEMENT_LIST
	: STATEMENT_LIST STATEMENT	{ $$ = new_ast_node(STATEMENT_LIST_CONST, $1, $2, args.inputFiles.currentFilename, yylineno); }
	|							{ $$ = NULL; }
	;

STATEMENT
	: if_statement				{ $$ = $1; }
	| iteration_statement		{ $$ = $1; }
	| return_statement			{ $$ = $1; }
	| declaration_statement		{ $$ = $1; }
	| assignment_statement		{ $$ = $1; }
	| function_call_statement	{ $$ = $1; }
	;

FUNCTION_DEFINITION
	: FUNCTION_RETURN_TYPE ID '(' FUNC_DECLARATION_ARG_LIST ')' '{' STATEMENT_LIST '}'	
								{ 
									AstNode *functionNode = new_ast_function_declaration_node($1, $2, $4, $7, args.inputFiles.currentFilename, yylineno);
									register_function(functionNode);
								}

	| FUNCTION_RETURN_TYPE ID '(' ')' '{' STATEMENT_LIST '}'							
								{ 
									AstNode *functionNode = new_ast_function_declaration_node($1, $2, NULL, $6, args.inputFiles.currentFilename, yylineno);
									register_function(functionNode);
								}
	;

BLOCK    	
	: '{' STATEMENT_LIST '}'	{ $$ = $2; }
	| STATEMENT 				{ $$ = $1; }
	| ';'						{ $$ = NULL; }
	;

if_statement
	: IF '(' NUM ')' BLOCK %prec IFX
								{ $$ = new_ast_if_node($3, $5, NULL, args.inputFiles.currentFilename, yylineno); }

	| IF '(' NUM ')' BLOCK ELSE BLOCK
								{ $$ = new_ast_if_node($3, $5, $7, args.inputFiles.currentFilename, yylineno); }
	;

iteration_statement
	: WHILE '(' NUM ')' BLOCK
								{ $$ = new_ast_while_node(WHILE, $3, $5, args.inputFiles.currentFilename, yylineno); }

	| DO BLOCK WHILE '(' NUM ')' ';'
								{ $$ = new_ast_while_node(DO, $5, $2, args.inputFiles.currentFilename, yylineno); }

	| FOR '(' DO_ASSIGNMENT ';' NUM ';' DO_ASSIGNMENT ')' BLOCK
								{ $$ = new_ast_for_node($3, $5, $7, $9, args.inputFiles.currentFilename, yylineno); }
	;

return_statement
	: RETURN ';'				{ $$ = new_ast_return_node(NULL, args.inputFiles.currentFilename, yylineno); }
	| RETURN VALUE ';'			{ $$ = new_ast_return_node($2, args.inputFiles.currentFilename, yylineno); }
	;

declaration_statement
	: ID_TYPE ID ';'			{ $$ = new_ast_declaration_node($1, $2, NULL, args.inputFiles.currentFilename, yylineno); }
	| ID_TYPE ID '=' VALUE ';'	{ $$ = new_ast_declaration_node($1, $2, $4, args.inputFiles.currentFilename, yylineno); }
	;

assignment_statement
	: ASSIGNMENT ';'			{ $$ = $1; }
	;

function_call_statement
	: FUNCTION_CALL ';'			{ $$ = $1; }

FUNCTION_CALL
	: ID '(' FUNC_ARG_LIST ')'	{ $$ = new_ast_function_call_node($1, $3, args.inputFiles.currentFilename, yylineno); }
	| ID '(' ')'				{ $$ = new_ast_function_call_node($1, NULL, args.inputFiles.currentFilename, yylineno); }
	;

ASSIGNMENT
	: ID '=' VALUE				{ $$ = new_ast_assignment_node($1, $3, args.inputFiles.currentFilename, yylineno); }
	| ID INC					{ $$ = new_ast_inc_dec_assignment_node($2, $1, args.inputFiles.currentFilename, yylineno); }
	| ID DEC					{ $$ = new_ast_inc_dec_assignment_node($2, $1, args.inputFiles.currentFilename, yylineno); }
	| SET_PROPERTY FROM ID '=' VALUE
								{ $$ = new_ast_set_property_node($3, $1, $5, args.inputFiles.currentFilename, yylineno); }

	| SET_NAMED_PROPERTY STRING_LITERAL FROM ID
								{ $$ = new_ast_set_named_property_node($4, $1, $2, NULL, yylineno); }

	| SET_NAMED_PROPERTY STRING_LITERAL FROM ID '=' VALUE
								{ $$ = new_ast_set_named_property_node($4, $1, $2, $6, args.inputFiles.currentFilename, yylineno); }

	| APPEND_CHILD FROM ID '=' VALUE
								{ $$ = new_ast_append_child_node($3, $5, args.inputFiles.currentFilename, yylineno); }
	;

SET_PROPERTY
	: SET BODY					{ $$ = $2; }
	| SET NAME					{ $$ = $2; }
	;

SET_NAMED_PROPERTY	
	: SET ATTRIBUTE				{ $$ = $2; }

DO_ASSIGNMENT
	: ASSIGNMENT				{ $$ = $1; }
	| ID_TYPE ID '=' VALUE		{ $$ = new_ast_declaration_node($1, $2, $4, args.inputFiles.currentFilename, yylineno); }
	|							{ $$ = NULL; }
	;

FUNC_DECLARATION_ARG_LIST
	: FUNC_DECLARATION_ARG_LIST ',' ID_TYPE ID 	
								{ 
								  ast_add_function_arg_declaration($1, $3, $4);
								  $$ = $1;  
								}

	| ID_TYPE ID				{ 
								  AstFunctionArgList *list = ast_create_function_arg_list();
								  ast_add_function_arg_declaration(list, $1, $2);
								  $$ = list;  
								}
	;

FUNC_ARG_LIST
	: FUNC_ARG_LIST ',' VALUE  	{ 
								  ast_add_function_arg_value($1, $3);
								  $$ = $1;  
								}

	| VALUE						{ 
								  AstFunctionArgList *list = ast_create_function_arg_list();
								  ast_add_function_arg_value(list, $1);
								  $$ = list;  
								}
	;
        
ARITH
	: NUM '+' NUM 				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '-' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '*' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '/' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '%' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '<' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM '>' NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM LE NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM GE NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM EQ NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM NE NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM OR NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| NUM AND NUM				{ $$ = new_ast_node($2, $1, $3, args.inputFiles.currentFilename, yylineno); }
	| '!' NUM					{ $$ = new_ast_node($1, $2, NULL, args.inputFiles.currentFilename, yylineno); }
	| '-' NUM %prec UMINUS		{ $$ = new_ast_node(UMINUS, $2, NULL, args.inputFiles.currentFilename, yylineno); }
	| '(' NUM ')'				{ $$ = $2; }
	;

NUM
	: ID						{ $$ = new_ast_symbol_reference_node($1, args.inputFiles.currentFilename, yylineno); }
	| FUNCTION_CALL				{ $$ = $1; }
	| NON_ID_NUM				{ $$ = $1; }
	;

NON_ID_NUM
	: INT_LITERAL				{ $$ = new_ast_int_node($1, args.inputFiles.currentFilename, yylineno); }
	| ARITH						{ $$ = $1; }
	| INT '(' STRING_VALUE ')' 	{ $$ = new_ast_node(INT_CAST_CONST, $3, NULL, args.inputFiles.currentFilename, yylineno); }
	| LEN '(' STRING_VALUE ')'	{ $$ = new_ast_node(LEN, $3, NULL, args.inputFiles.currentFilename, yylineno); }
	| CMP '(' STRING_VALUE ',' STRING_VALUE ')'
								{ $$ = new_ast_node(CMP, $3, $5, args.inputFiles.currentFilename, yylineno); }
	;

STRING_ARITH
	: CONCAT '(' STRING_VALUE ',' STRING_VALUE ')'
								{ $$ = new_ast_node(CONCAT, $3, $5, args.inputFiles.currentFilename, yylineno); }
	;

STRING_VALUE
	: ID						{ $$ = new_ast_symbol_reference_node($1, args.inputFiles.currentFilename, yylineno); }
	| FUNCTION_CALL				{ $$ = $1; }
	| NON_ID_STRING_VALUE		{ $$ = $1; }
	;

NON_ID_STRING_VALUE
	: STRING_LITERAL			{ $$ = new_ast_string_node($1, args.inputFiles.currentFilename, yylineno); }
	| STRING_ARITH				{ $$ = $1; }
	| STR '(' NUM ')'			{ $$ = new_ast_node(STR, $3, NULL, args.inputFiles.currentFilename, yylineno); }
	| GET_PROPERTY FROM ID		{ $$ = new_ast_get_property_node($3, $1, args.inputFiles.currentFilename, yylineno); }
	| GET_NAMED_PROPERTY STRING_LITERAL FROM ID
								{ $$ = new_ast_get_named_property_node($4, $1, $2, args.inputFiles.currentFilename, yylineno); }
	;

VALUE
	: ID						{ $$ = new_ast_symbol_reference_node($1, args.inputFiles.currentFilename, yylineno); }
	| FUNCTION_CALL				{ $$ = $1; }
	| NON_ID_STRING_VALUE		{ $$ = $1; }
	| NON_ID_NUM				{ $$ = $1; }
	| NEW TAG					{ $$  = new_ast_tag_node(yylineno); }
	;

GET_PROPERTY
	: GET BODY					{ $$ = $2; }
	| GET NAME					{ $$ = $2; }
	;

GET_NAMED_PROPERTY	
	: GET ATTRIBUTE				{ $$ = $2; }

ID_TYPE
	: INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	| TAG						{ $$ = $1; }
	;

FUNCTION_RETURN_TYPE
	: VOID						{ $$ = $1; }
	| INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	| TAG						{ $$ = $1; }
	;

%%

int main(int argc, char *argv[]) {

	parse_args(argc, argv);

	FILE *output = initialize();

	parse_input_files();

	Tag * tag = execute_main();

	printf("All input files excecuted successfully. Proceeding to render main tag\n");
	
	int status = render_main_tag(tag, output);

	if(status == 0)
		printf("Main tag rendered successfully to output file %s.\n", args.outputFileName);

	fclose(output);

	finalize(status);
}
