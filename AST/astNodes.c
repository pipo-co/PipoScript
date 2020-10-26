
#include "astNodes.h"



AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right) {

    AstNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = nodeType;

    astNode->left = left;
    astNode->right = right;

    return astNode;
    }

AstNode * new_ast_symbol_reference_node(char *symbolName) {

    AstSymbolReferenceNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = ID;

    astNode->symbolName = symbolName;

    return (AstNode *) astNode;
}

AstNode * new_ast_if_node(AstNode * condition, AstNode * ifBranch, AstNode * elseBranch) {

    AstIfNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = IF;

    astNode->condition = condition;
    astNode->ifBranch = ifBranch;
    astNode->elseBranch = elseBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_while_node(int nodeType, AstNode *condition, AstNode *whileBranch) {

    if(nodeType != WHILE && nodeType != DO) {
        fprintf(stderr, "new_ast_while_node: Invalid type %d. Only valid %d (WHILE) and %d (DO)", nodeType, WHILE, DO);
        abort();
    }

    AstWhileNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = WHILE;

    astNode->condition = condition;
    astNode->whileBranch = whileBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch) {

    AstForNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FOR;

    astNode->firstAssignment = firstAssignment;
    astNode->condition = condition;
    astNode->lastAssignment = lastAssignment;
    astNode->forBranch = forBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_declaration_node(int type, char *symbolName, AstNode *value) {

    AstDeclarationNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = DECLARATION;

    astNode->type = type;
    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_assignment_node(char *symbolName, AstNode *value) {

    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = '=';

    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_inc_dec_assignment_node(int type, char *symbolName) {
    
    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = type;

    astNode->symbolName = symbolName;
    astNode->value = NULL;

    return (AstNode *) astNode;
}

AstNode * new_ast_int_node(int value) {

    AstIntNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = INT;

    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_string_node(char *value) {

    AstStringNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = STRING;

    astNode->value = value;

    return (AstNode *) astNode;
}

#include "astNodeFunctions.c"

AstNode * execute_ast_tree(AstNode *root, SymbolTable st) {

    if(root == NULL)
        return NULL;

    int nodeType = root->nodeType;

    return nodeProcessors[AST_OP_POSITION(nodeType)](root, st);
}

void free_ast_tree(AstNode *root) {
    return;
}