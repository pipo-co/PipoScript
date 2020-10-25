DIGIT			[0-9]
LETTER			[a-zA-Z_]
HEX				[a-fA-F0-9]

%{

#include <stdio.h>
#include "y.tab.h"

void multiLineComment(void);

%}

%option yylineno

%option nodefault

%%
"/*"							{ multiLineComment(); }
"//"[^\n]*              		{ /* consume //-comment */ }

"do"							{ return DO; }
"else"							{ return ELSE; }
"for"							{ return FOR; }
"if"							{ return IF; }
"int"							{ return INT; }
"string"						{ return STRING; }
"return"						{ return RETURN; }
"void"							{ return VOID; }
"while"							{ return WHILE; }

{LETTER}({LETTER}|{DIGIT})*		{ return ID; }

0|[1-9]{DIGIT}*					{ return INT_LITERAL; }
\"(\\.|[^\\"\n])*\"				{ return STRING_LITERAL; }


">>"							{ return RIGHT; }
"<<"							{ return LEFT; }
"++"							{ return INC; }
"--"							{ return DEC; }


"&&"							{ return AND; }
"||"							{ return OR; }
"<="							{ return LE; }
">="							{ return GE; }
"=="							{ return EQ; }
"!="							{ return NE; }

[ \t\v\n\f]						{ }

.								{ return yytext[0]; }

%%

int yywrap(void) {
	return 1;
}

int main (void) {
	yyparse();
	return 0;
}

void multiLineComment(void) {
	char c, prev = 0;

	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;

		prev = c;

	}
	printf("Error in line %d : unterminated comment\n", yylineno);
	exit(2);
}
