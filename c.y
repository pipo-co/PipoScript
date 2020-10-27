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

%error-verbose

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
%nonassoc STATEMENT_LIST_CONST

%type <astNode> STATEMENT_LIST STATEMENT BLOCK if_statement iteration_statement return_statement declaration_statement
%type <astNode> assignment_statement ASSIGNMENT DO_ASSIGNMENT ARITH NUM NON_ID_NUM VALUE 

%type <operation> ID_TYPES FUNCTION_TYPES

%token DECLARATION

%token LAST

%start P

%%

P       
	: STATEMENT_LIST
		{ 
			if(yychar == YYEOF) {
				printf("Input accepted\n");
			 	astTree = $1;
			}
			else
				yyerror("Unexpected end of program. Invalid statement.");
		}
	;

STATEMENT_LIST
	: STATEMENT_LIST STATEMENT	{ $$ = new_ast_node(STATEMENT_LIST_CONST, $1, $2); }
	|							{ $$ = NULL; }
	;

STATEMENT
	: if_statement				{ $$ = $1; }
	| iteration_statement		{ $$ = $1; }
	| return_statement			{ $$ = $1; }
	| declaration_statement		{ $$ = $1; }
	| assignment_statement		{ $$ = $1; }
	;

BLOCK    	
	: '{' STATEMENT_LIST '}'	{ $$ = $2; }
	| STATEMENT 				{ $$ = $1; }
	| ';'						{ $$ = NULL; }
	;

if_statement
	: IF '(' NUM ')' BLOCK %prec IFX
								{ $$ = new_ast_if_node($3, $5, NULL); }

	| IF '(' NUM ')' BLOCK ELSE BLOCK
								{ $$ = new_ast_if_node($3, $5, $7); }
	;

iteration_statement
	: WHILE '(' NUM ')' BLOCK
								{ $$ = new_ast_while_node(WHILE, $3, $5); }

	| DO BLOCK WHILE '(' NUM ')' ';'
								{ $$ = new_ast_while_node(DO, $5, $2); }

	| FOR '(' DO_ASSIGNMENT ';' NUM ';' DO_ASSIGNMENT ')' BLOCK
								{ $$ = new_ast_for_node($3, $5, $7, $9); }
	;

return_statement
	: RETURN ';'				{ $$ = NULL; }
	| RETURN VALUE ';'			{ $$ = NULL; }
	;

declaration_statement
	: ID_TYPES ID ';'			{ $$ = new_ast_declaration_node($1, $2, NULL); }
	| ID_TYPES ID '=' VALUE ';'	{ $$ = new_ast_declaration_node($1, $2, $4); }
	;

assignment_statement
	: ASSIGNMENT ';'			{ $$ = $1; }
	;

ASSIGNMENT
	: ID '=' VALUE				{ $$ = new_ast_assignment_node($1, $3); }
	| ID INC					{ $$ = new_ast_inc_dec_assignment_node($2, $1); }
	| ID DEC					{ $$ = new_ast_inc_dec_assignment_node($2, $1); }
	;

DO_ASSIGNMENT
	: ASSIGNMENT				{ $$ = $1; }
	| ID_TYPES ID '=' VALUE		{ $$ = new_ast_declaration_node($1, $2, $4); }
	|							{ $$ = NULL; }
	;
        
ARITH
	: NUM '+' NUM 				{ $$ = new_ast_node($2, $1, $3); }
	| NUM '-' NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM '*' NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM '/' NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM '<' NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM '>' NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM LE NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM GE NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM EQ NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM NE NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM OR NUM				{ $$ = new_ast_node($2, $1, $3); }
	| NUM AND NUM				{ $$ = new_ast_node($2, $1, $3); }
	| '!' NUM					{ $$ = new_ast_node($1, $2, NULL); }
	| '-' NUM %prec UMINUS		{ $$ = new_ast_node(UMINUS, $2, NULL); }
	| '(' NUM ')'				{ $$ = $2; }
	;

NUM
	: ID						{ $$ = new_ast_symbol_reference_node($1); }
	| NON_ID_NUM				{ $$ = $1; }
	;

NON_ID_NUM
	: INT_LITERAL				{ $$ = new_ast_int_node($1); }
	| ARITH						{ $$ = $1; }
	;

VALUE
	: ID						{ $$ = new_ast_symbol_reference_node($1); }
	| STRING_LITERAL			{ $$ = new_ast_string_node($1); }
	| NON_ID_NUM				{ $$ = $1; }
	;

ID_TYPES
	: INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	;

FUNCTION_TYPES
	: VOID						{ $$ = $1; }
	| INT						{ $$ = $1; }
	| STRING					{ $$ = $1; }
	;

%%

int main(void) {

	// extern FILE *yyin, *yyout; 
  
    // yyin = fopen("test.c", "r");

	// yyout = fopen("result.txt", "w");

	initialize();

	yyparse();

	execute_ast_tree(astTree, globalSt);

	finalize();

	return 0;
}
