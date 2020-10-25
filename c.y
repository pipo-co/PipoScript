%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "astNodes.h"

	extern char yytext[];
	extern int yylineno;

	void yyerror(char const *s);
	int yylex();

	AstNode *astTree;

	int sym[26]; 
%}

%error-verbose

%token ID INT_LITERAL STRING_LITERAL
%token INC DEC LEFT RIGHT LE GE EQ NE
%token AND OR

%token INT VOID STRING

%token IF ELSE WHILE DO FOR RETURN

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

%nonassoc IFX
%nonassoc ELSE
%nonassoc STATEMENT_LIST_CONST

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
								{ $$ = new_ast_if_node ($3, $5, NULL); }

	| IF '(' NUM ')' BLOCK ELSE BLOCK
								{ $$ = new_ast_if_node ($3, $5, $7); }
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
	: RETURN ';'
	| RETURN VALUE ';'
	;

declaration_statement
	: ID_TYPES ID ';'			{ //declare }
	| ID_TYPES ID '=' VALUE ';'	{ //declare and fill }
	;

assignment_statement
	: ASSIGNMENT ';'			{ $$ = $1; }
	;

ASSIGNMENT
	: ID '=' VALUE				{ //$$ = new_ast_assignment_node(lookup($1), $3); }
	| ID INC					{ $$ = new_ast_inc_assignment_node($1); }
	| ID DEC					{ $$ = new_ast_dec_assignment_node($1); }
	;

DO_ASSIGNMENT
	: ASSIGNMENT				{ $$ = $1; }
	| ID_TYPES ASSIGNMENT		{ $$ = $1; //TODO }
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
	: ID						{ //$$ = new_ast_symbol_reference_node(lookup($1)); }
	| NON_ID_NUM				{ $$ = $1; }
	;

NON_ID_NUM
	: INT_LITERAL				{ $$ = new_ast_int_node($1); }
	| ARITH						{ $$ = $1; }
	;

VALUE
	: ID						{ //$$ = new_ast_symbol_reference_node(lookup($1)); }
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


void yyerror(char const *s)
{
	fprintf(stderr, "Error in line %d: ", yylineno);
	fprintf(stderr, "\"%s\"\n",s);
	exit(3);
}

int main(void) {

	yyparse();

	// execute(astTree);

	// ast_tree_free(astTree);

	return 0;
}
