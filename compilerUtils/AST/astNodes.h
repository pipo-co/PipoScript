#ifndef AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1
#define AST_NODES_H_fc51f24f6009d9dada1771be6189418224888db1

#include "compilerUtils/symbolTable/symbolTable.h"
#include "pipoScriptFunctions/tag.h"

#define MAX_FUNC_ARGS 30

// Generic AstNode
typedef struct AstNode {
  int nodeType;
  int lineno;

  struct AstNode * left;

  struct AstNode * right;
} AstNode;

typedef struct AstFunctionArgNode {

  int type;
  char* symbolName;
  AstNode *value;

  struct AstFunctionArgNode *next;
} AstFunctionArgNode;

typedef struct AstFunctionArgList {

  AstFunctionArgNode *first;
  AstFunctionArgNode *last;
  int argCount;
  
} AstFunctionArgList;

#include "y.tab.h"

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

typedef struct AstFunctionDeclarationNode {
  int nodeType;
  int lineno;

  int returnType;
  char *functionName;

  AstFunctionArgList *args;

  AstNode * block;
} AstFunctionDeclarationNode;

typedef struct AstFunctionCallNode {
  int nodeType;
  int lineno;

  char *functionName;
  AstFunctionArgList *args;

} AstFunctionCallNode;

typedef struct AstReturnNode {
  int nodeType;
  int lineno;

  AstNode *value;
} AstReturnNode;

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

typedef struct AstSetPropertyNode {
  int nodeType;
  int lineno;

  int propertyType;
  char *symbolName;
  AstNode *value;

} AstSetPropertyNode;

typedef struct AstSetNamedPropertyNode {
  int nodeType;
  int lineno;

  int propertyType;
  char *symbolName;
  char *propertyName;
  AstNode *value;

} AstSetNamedPropertyNode;

typedef struct AstAppendChildNode {
  int nodeType;
  int lineno;

  char *symbolName;
  AstNode *value;

} AstAppendChildNode;

typedef struct AstGetPropertyNode {
  int nodeType;
  int lineno;

  int propertyType;
  char *symbolName;

} AstGetPropertyNode;

typedef struct AstGetNamedPropertyNode {
  int nodeType;
  int lineno;

  int propertyType;
  char *symbolName;
  char *propertyName;

} AstGetNamedPropertyNode;

void initialize_ast_node_functions();

Tag * execute_ast_tree(AstNode *tree, SymbolTable st);

void free_ast_tree(AstNode *root);

AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right, int lineno);

AstNode * new_ast_symbol_reference_node(char *symbolName, int lineno);

AstNode * new_ast_if_node (AstNode * condition, AstNode * ifBranch, AstNode * elseBranch, int lineno);

AstNode * new_ast_while_node(int nodeType, AstNode * condition, AstNode * whileBranch, int lineno);

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch, int lineno);

AstNode * new_ast_declaration_node(int type, char *symbolName, AstNode *value, int lineno);

AstNode * new_ast_assignment_node(char *symbolName, AstNode *value, int lineno);

AstNode * new_ast_inc_dec_assignment_node(int type, char *symbolName, int lineno);

AstNode * new_ast_set_property_node(char *symbolName, int propertyType, AstNode *value, int lineno);

AstNode * new_ast_set_named_property_node(char *symbolName, int propertyType, char *propertyName, AstNode *value, int lineno);

AstNode * new_ast_append_child_node(char *symbolName, AstNode *value, int lineno);

AstNode * new_ast_get_property_node(char *symbolName, int propertyType, int lineno);

AstNode * new_ast_get_named_property_node(char *symbolName, int propertyType, char *propertyName, int lineno);

AstNode * new_ast_int_node(int value, int lineno);

AstNode * new_ast_string_node(char *value, int lineno);

AstNode * new_ast_return_node(AstNode *value, int lineno);

AstNode * new_ast_function_declaration_node(int returnType, char *name, AstFunctionArgList *args, AstNode *block, int lineno);

AstNode * new_ast_function_call_node(char *name, AstFunctionArgList *args, int lineno);

// Function Auxiliary Functions
AstFunctionArgList *ast_create_function_arg_list();

AstFunctionArgNode * ast_add_function_arg_declaration(AstFunctionArgList *list, int type, char* name);

AstFunctionArgNode * ast_add_function_arg_value(AstFunctionArgList *list, AstNode *value);

void ast_free_function_arg_list(AstFunctionArgList *list);

#endif