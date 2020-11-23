#include "astNodes.h"

AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right, int lineno) {

    AstNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = nodeType;
    astNode->lineno = lineno;

    astNode->left = left;
    astNode->right = right;

    return astNode;
    }

AstNode * new_ast_symbol_reference_node(char *symbolName, int lineno) {

    AstSymbolReferenceNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = ID;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;

    return (AstNode *) astNode;
}

AstNode * new_ast_if_node(AstNode * condition, AstNode * ifBranch, AstNode * elseBranch, int lineno) {

    AstIfNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = IF;
    astNode->lineno = lineno;

    astNode->condition = condition;
    astNode->ifBranch = ifBranch;
    astNode->elseBranch = elseBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_while_node(int nodeType, AstNode *condition, AstNode *whileBranch, int lineno) {

    if(nodeType != WHILE && nodeType != DO) {
        fprintf(stderr, "new_ast_while_node: Invalid type %d. Only valid %d (WHILE) and %d (DO)", nodeType, WHILE, DO);
        abort();
    }

    AstWhileNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = WHILE;
    astNode->lineno = lineno;

    astNode->condition = condition;
    astNode->whileBranch = whileBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch, int lineno) {

    AstForNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FOR;
    astNode->lineno = lineno;

    astNode->firstAssignment = firstAssignment;
    astNode->condition = condition;
    astNode->lastAssignment = lastAssignment;
    astNode->forBranch = forBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_declaration_node(int type, char *symbolName, AstNode *value, int lineno) {

    AstDeclarationNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = DECLARATION;
    astNode->lineno = lineno;

    astNode->type = type;
    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_assignment_node(char *symbolName, AstNode *value, int lineno) {

    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = '=';
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_inc_dec_assignment_node(int type, char *symbolName, int lineno) {
    
    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = type;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;
    astNode->value = NULL;

    return (AstNode *) astNode;
}

AstNode * new_ast_int_node(int value, int lineno) {

    AstIntNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = INT;
    astNode->lineno = lineno;

    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_string_node(char *value, int lineno) {

    AstStringNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = STRING;
    astNode->lineno = lineno;

    astNode->value = value;

    return (AstNode *) astNode;
}

#include "astNodeFunctions.c"

void execute_ast_tree(AstNode *tree, SymbolTable st) {
    execute_ast_node(tree, st);
}

void free_ast_tree(AstNode *node) {

    if(node == NULL)
        return;

    int nodeType = node->nodeType;

    astNodeFunctions[AST_OP_POSITION(nodeType)].destroyer(node);
}