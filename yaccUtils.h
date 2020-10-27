#ifndef YACC_UTILS_H_27421fb4f84edd2899a18f724ae8ca2e88510203
#define YACC_UTILS_H_27421fb4f84edd2899a18f724ae8ca2e88510203

#include <stdio.h>
#include <stdlib.h>

#include "AST/astNodes.h"
#include "y.tab.h"

#include "symbolTable/symbolTable.h"

// Lex global variables
extern char yytext[];
extern int yylineno;

// Lexer
int yylex();

// Global variables
AstNode *astTree;
SymbolTable globalSt;


void yyerror(char const *s);

void initialize(void);

void finalize(void);

#endif