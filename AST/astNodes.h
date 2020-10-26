#ifndef AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1
#define AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1

#include <stdlib.h>
#include <stdio.h>

#include "../pipoUtils/pipoUtils.h"
#include "../symbolTable/symbolTable.h"

// Generic AstNode
typedef struct AstNode {
  int nodeType;

  struct AstNode * left;

  struct AstNode * right;
} AstNode;

#include "../y.tab.h"

typedef struct AstIfNode {
  int nodeType;

  struct AstNode * condition;

  struct AstNode * ifBranch;

  struct AstNode * elseBranch;
} AstIfNode;

typedef struct AstWhileNode {
  int nodeType;

  struct AstNode * condition;

  struct AstNode * whileBranch;
} AstWhileNode;

typedef struct AstForNode {
  int nodeType;

  struct AstNode * firstAssignment;

  struct AstNode * condition;

  struct AstNode * lastAssignment;

  struct AstNode * forBranch;
} AstForNode;

typedef struct AstSymbolReferenceNode {
  int nodeType;
  
  SymbolNode * symbol;
} AstSymbolReferenceNode;

typedef struct AstAssignmentNode {
  int nodeType;

  struct SymbolNode * symbol;

  struct AstNode * value;
} AstAssignmentNode;

typedef struct AstIntNode {
  int nodeType;

  double value;
} AstIntNode;

typedef struct AstStringNode {
  int nodeType;

  char *value;
} AstStringNode;

void free_ast_tree(AstNode *root);

AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right);

AstNode * new_ast_symbol_reference_node(SymbolNode * symbol);

AstNode * new_ast_if_node (AstNode * condition, AstNode * ifBranch, AstNode * elseBranch);

AstNode * new_ast_while_node(int nodeType, AstNode * condition, AstNode * whileBranch);

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch);

AstNode * new_ast_assignment_node(SymbolNode * symbol, AstNode * value);

AstNode * new_ast_int_node(int value);

AstNode * new_ast_string_node(char *value);



#endif