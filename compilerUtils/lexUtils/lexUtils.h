#ifndef LEX_UTILS_H_edc601e6c39f50cc50daa04ecad35776a214e749
#define LEX_UTILS_H_edc601e6c39f50cc50daa04ecad35776a214e749

#include <stdio.h>
#include <string.h>

#include "compilerUtils/AST/astNodes.h"
#include "y.tab.h"

#include "utils/errorHandling.h"

extern int yylineno;

void initializeLex();

void finalizeLex();

void multiLineComment(int (*input)(void));

char * lex_copy_string(char* string, int len);

#endif