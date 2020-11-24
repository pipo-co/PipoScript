#ifndef YACC_UTILS_H_27421fb4f84edd2899a18f724ae8ca2e88510203
#define YACC_UTILS_H_27421fb4f84edd2899a18f724ae8ca2e88510203

#include <stdio.h>
#include <stdlib.h>

#include "compilerUtils/AST/astNodes.h"
#include "y.tab.h"

#include "compilerUtils/symbolTable/symbolTable.h"
#include "compilerUtils/functionSymbolTable/functionSymbolTable.h"
#include "pipoScriptFunctions/tag.h"

// Lex global variables
extern char yytext[];
extern int yylineno;

// Lexer
int yylex();

// Global variables
extern AstNode *astTree;
extern SymbolTable globalSt;


// Yacc Error Function
void yyerror(char const *s);

void initialize(void);

Tag * execute_main(void);

void render_final_tag(Tag * tag, FILE * out);

void finalize(int status);

void register_function(AstNode *node);

#endif