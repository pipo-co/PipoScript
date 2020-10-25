%{
	#include <stdio.h>
	#include <stdlib.h>

	extern char yytext[];
	extern int yylineno;

	void yyerror(char const *s);
	int yylex();

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

%start P

%%

P       
	: STATEMENT_LIST
		{ 
			if(yychar == YYEOF) {
				printf("%d\n", yylen);
				printf("%s\n", yytext);
				printf("Input accepted\n");
			 	exit(0);
			}
			else
				yyerror("Unexpected end of program. Invalid statement.");
		}
	;

STATEMENT_LIST
	: STATEMENT_LIST STATEMENT
	|
	;

STATEMENT
	: if_statement
	| iteration_statement
	| return_statement
	| declaration_statement
	| assignment_statement
	;

BLOCK    	
	: '{' STATEMENT_LIST '}'
	| STATEMENT 
	| ';'
	;

if_statement
	: IF '(' NUM ')' BLOCK %prec IFX
	| IF '(' NUM ')' BLOCK ELSE BLOCK
	;

iteration_statement
	: WHILE '(' NUM ')' BLOCK
	| DO BLOCK WHILE '(' NUM ')' ';'
	| FOR '(' DO_ASSIGNMENT ';' NUM ';' DO_ASSIGNMENT ')' BLOCK
	;

return_statement
	: RETURN ';'
	| RETURN VALUE ';'
	;

declaration_statement
	: ID_TYPES ID ';'
	| ID_TYPES ID '=' VALUE ';'
	;

assignment_statement
	: ASSIGNMENT ';'
	;

ASSIGNMENT
	: ID '=' VALUE
	| ID INC
	| ID DEC
	;

DO_ASSIGNMENT
	: ASSIGNMENT
	| ID_TYPES ASSIGNMENT
	|
	;
        
ARITH
	: NUM '+' NUM 
	| NUM '-' NUM
	| NUM '*' NUM
	| NUM '/' NUM
	| NUM '<' NUM
	| NUM '>' NUM
	| NUM LE NUM
	| NUM GE NUM
	| NUM EQ NUM
	| NUM NE NUM
	| NUM OR NUM
	| NUM AND NUM
	| '!' NUM
	| '-' NUM %prec UMINUS
	| '(' NUM ')'
	;

NUM
	: ID
	| NON_ID_NUM
	;

NON_ID_NUM
	: INT_LITERAL
	| ARITH
	;

VALUE
	: ID
	| STRING_LITERAL
	| NON_ID_NUM
	;

ID_TYPES
	: INT
	| STRING
	;

FUNCTION_TYPES
	: VOID
	| INT
	| STRING
	;

%%


void yyerror(char const *s)
{
	fprintf(stderr, "Error in line %d: ", yylineno);
	fprintf(stderr, "\"%s\"\n",s);
	exit(3);
}

int main(void) {

	printf("int main() {\n");


	yyparse();
	return 0;
}
