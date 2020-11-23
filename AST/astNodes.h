#ifndef AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1
#define AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1

#include <stdlib.h>
#include <stdio.h>

#include "../pipoUtils/pipoUtils.h"
#include "../symbolTable/symbolTable.h"

// Generic AstNode
typedef struct AstNode {
  int nodeType;
  int lineno;

  struct AstNode * left;

  struct AstNode * right;
} AstNode;

#include "../y.tab.h"

typedef struct AstIfNode {
  int nodeType;
  int lineno;

  AstNode * condition;

  AstNode * ifBranch;

  AstNode * elseBranch;
} AstIfNode;

typedef struct AstWhileNode {
  int nodeType;
  int lineno;

  AstNode * condition;

  AstNode * whileBranch;
} AstWhileNode;

typedef struct AstForNode {
  int nodeType;
  int lineno;

  AstNode * firstAssignment;

  AstNode * condition;

  AstNode * lastAssignment;

  AstNode * forBranch;
} AstForNode;

typedef struct AstSymbolReferenceNode {
  int nodeType;
  int lineno;
  
  char *symbolName;
} AstSymbolReferenceNode;

typedef struct AstAssignmentNode {
  int nodeType;
  int lineno;

  char *symbolName;

  AstNode * value;
} AstAssignmentNode;

typedef struct AstDeclarationNode {
  int nodeType;
  int lineno;

  int type;

  char *symbolName;

  AstNode * value;
} AstDeclarationNode;

typedef struct AstIntNode {
  int nodeType;
  int lineno;

  int value;
} AstIntNode;

typedef struct AstStringNode {
  int nodeType;
  int lineno;

  char *value;
} AstStringNode;

void initialize_ast_node_functions();

void execute_ast_tree(AstNode *tree, SymbolTable st);

void free_ast_tree(AstNode *root);

AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right, int lineno);

AstNode * new_ast_symbol_reference_node(char *symbolName, int lineno);

AstNode * new_ast_if_node (AstNode * condition, AstNode * ifBranch, AstNode * elseBranch, int lineno);

AstNode * new_ast_while_node(int nodeType, AstNode * condition, AstNode * whileBranch, int lineno);

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch, int lineno);

AstNode * new_ast_declaration_node(int type, char *symbolName, AstNode *value, int lineno);

AstNode * new_ast_assignment_node(char *symbolName, AstNode *value, int lineno);

AstNode * new_ast_inc_dec_assignment_node(int type, char *symbolName, int lineno);

AstNode * new_ast_int_node(int value, int lineno);

AstNode * new_ast_string_node(char *value, int lineno);



#endif