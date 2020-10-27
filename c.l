DIGIT			[0-9]
LETTER			[a-zA-Z_]
HEX				[a-fA-F0-9]

%{

	#include "lexUtils.h"

%}

%option yylineno

%option nodefault

%%
"/*"							{ multiLineComment(input); }
"//"[^\n]*              		{ /* consume //-comment */ }

"do"							{ yylval.operation = DO; return DO; }
"else"							{ yylval.operation = ELSE; return ELSE; }
"for"							{ yylval.operation = FOR; return FOR; }
"if"							{ yylval.operation = IF; return IF; }
"int"							{ yylval.operation = INT; return INT; }
"string"						{ yylval.operation = STRING; return STRING; }
"return"						{ yylval.operation = RETURN; return RETURN; }
"void"							{ yylval.operation = VOID; return VOID; }
"while"							{ yylval.operation = WHILE; return WHILE; }

{LETTER}({LETTER}|{DIGIT})*		{ yylval.symbol = lex_copy_string(yytext, yyleng); return ID; }

0|[1-9]{DIGIT}*					{ yylval.intValue = atoi(yytext); return INT_LITERAL; }
\"(\\.|[^\\"\n])*\"				{ yylval.stringValue = lex_copy_string(yytext + 1, yyleng - 2); return STRING_LITERAL; }


">>"							{ yylval.operation = RIGHT; return RIGHT; }
"<<"							{ yylval.operation = LEFT; return LEFT; }
"++"							{ yylval.operation = INC; return INC; }
"--"							{ yylval.operation = DEC; return DEC; }


"&&"							{ yylval.operation = AND; return AND; }
"||"							{ yylval.operation = OR; return OR; }
"<="							{ yylval.operation = LE; return LE; }
">="							{ yylval.operation = GE; return GE; }
"=="							{ yylval.operation = EQ; return EQ; }
"!="							{ yylval.operation = NE; return NE; }

[ \t\v\n\f]						{ }

.								{ yylval.operation = yytext[0]; return yytext[0]; }

%%

int yywrap(void) {
	return 1;
}
