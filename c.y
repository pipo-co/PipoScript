%{

	#include "yaccUtils.h"

%}

%union {

	int operation;

	int intValue;

	char *stringValue;

	char *symbol;

	AstNode *astNode;
}

// %error-verbose

%token <operation> FIRST

%token <symbol> ID

%token <intValue> INT_LITERAL
%token <stringValue> STRING_LITERAL

%token <operation> LEFT RIGHT
%token <operation> INT VOID STRING
%token <operation> IF ELSE WHILE DO FOR RETURN

%right '='
%left OR AND
%left '>' '<' LE GE EQ NE
%left '+' '-'
%left '*' '/'
%right '!'
%left INC DEC
%left ')'
%right '('
%nonassoc UMINUS

%type <operation> AND OR '=' '>' '<' LE GE EQ NE '+' '-' '*' '/' '!' INC DEC ')' '(' UMINUS

%nonassoc IFX
%nonassoc ELSE
%nonassoc STATEMENT_LIST_CONST FUNCTION_DECLARATION_CONST FUNCTION_CALL_CONST FUNCTION_DEFINITION_LIST_CONST

%type <astNode> STATEMENT_LIST STATEMENT BLOCK if_statement iteration_statement return_statement declaration_statement
%type <astNode> assignment_statement ASSIGNMENT DO_ASSIGNMENT ARITH NUM NON_ID_NUM VALUE 
%type <astNode> FUNC_DECLARATION_ARG_LIST FUNC_ARG_LIST function_call_statement

%type <operation> ID_TYPE FUNCTION_RETURN_TYPE

%token DECLARATION

%token LAST

%start P

%%

P       
	: FUNCTION_DEFINITION_LIST
		{ 
			if(yychar == YYEOF) {
				printf("Input accepted\n");
			}
			else
				yyerror("Unexpected end of program");
		}
	;

FUNCTION_DEFINITION_LIST
	: FUNCTION_DEFINITION_LIST FUNCTION_DEFINITION
	|
	;

STATEMENT_LIST
	: STATEMENT_LIST STATEMENT	{ $$ = new_ast_node(STATEMENT_LIST_CONST, $1, $2, yylineno); }
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
									AstNode *functionNode = new_ast_function_declaration_node($1, $2, $4, $7, yylineno);
									register_function(functionNode);
								}

	| FUNCTION_RETURN_TYPE ID '(' ')' '{' STATEMENT_LIST '}'							
								{ 
									AstNode *functionNode = new_ast_function_declaration_node($1, $2, NULL, $6, yylineno);
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
								{ $$ = new_ast_if_node($3, $5, NULL, yylineno); }

	| IF '(' NUM ')' BLOCK ELSE BLOCK
								{ $$ = new_ast_if_node($3, $5, $7, yylineno); }
	;

iteration_statement
	: WHILE '(' NUM ')' BLOCK
								{ $$ = new_ast_while_node(WHILE, $3, $5, yylineno); }

	| DO BLOCK WHILE '(' NUM ')' ';'
								{ $$ = new_ast_while_node(DO, $5, $2, yylineno); }

	| FOR '(' DO_ASSIGNMENT ';' NUM ';' DO_ASSIGNMENT ')' BLOCK
								{ $$ = new_ast_for_node($3, $5, $7, $9, yylineno); }
	;

return_statement
	: RETURN ';'				{ $$ = NULL; }
	| RETURN VALUE ';'			{ $$ = NULL; }
	;

declaration_statement
	: ID_TYPE ID ';'			{ $$ = new_ast_declaration_node($1, $2, NULL, yylineno); }
	| ID_TYPE ID '=' VALUE ';'	{ $$ = new_ast_declaration_node($1, $2, $4, yylineno); }
	;

assignment_statement
	: ASSIGNMENT ';'			{ $$ = $1; }
	;

function_call_statement
	: ID '(' FUNC_ARG_LIST ')' ';'	
								{ $$ = new_ast_function_call_node($1, $2, yylineno); }

	| ID '(' ')' ';'
								{ $$ = new_ast_function_call_node($1, NULL, yylineno); }
	;

ASSIGNMENT
	: ID '=' VALUE				{ $$ = new_ast_assignment_node($1, $3, yylineno); }
	| ID INC					{ $$ = new_ast_inc_dec_assignment_node($2, $1, yylineno); }
	| ID DEC					{ $$ = new_ast_inc_dec_assignment_node($2, $1, yylineno); }
	;

DO_ASSIGNMENT
	: ASSIGNMENT				{ $$ = $1; }
	| ID_TYPE ID '=' VALUE		{ $$ = new_ast_declaration_node($1, $2, $4, yylineno); }
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
	: NUM '+' NUM 				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM '-' NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM '*' NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM '/' NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM '<' NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM '>' NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM LE NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM GE NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM EQ NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM NE NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM OR NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| NUM AND NUM				{ $$ = new_ast_node($2, $1, $3, yylineno); }
	| '!' NUM					{ $$ = new_ast_node($1, $2, NULL, yylineno); }
	| '-' NUM %prec UMINUS		{ $$ = new_ast_node(UMINUS, $2, NULL, yylineno); }
	| '(' NUM ')'				{ $$ = $2; }
	;

NUM
	: ID						{ $$ = new_ast_symbol_reference_node($1, yylineno); }
	| NON_ID_NUM				{ $$ = $1; }
	;

NON_ID_NUM
	: INT_LITERAL				{ $$ = new_ast_int_node($1, yylineno); }
	| ARITH						{ $$ = $1; }
	;

VALUE
	: ID						{ $$ = new_ast_symbol_reference_node($1, yylineno); }
	| STRING_LITERAL			{ $$ = new_ast_string_node($1, yylineno); }
	| NON_ID_NUM				{ $$ = $1; }
	;

ID_TYPE
	: INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	;

FUNCTION_RETURN_TYPE
	: VOID						{ $$ = $1; }
	| INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	;

%%

int main(void) {

	extern FILE *yyin, *yyout; 
  
    yyin = fopen("test.c", "r");

	yyout = fopen("result.txt", "w");

	initialize();

	yyparse();

	int status = execute_main();

	finalize(status);
}
