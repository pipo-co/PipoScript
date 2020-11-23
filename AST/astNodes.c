#include "astNodes.h"

#include <stdlib.h>
#include <stdio.h>

#include "../pipoUtils/pipoUtils.h"


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

AstNode * new_ast_function_declaration_node(int returnType, char *name, AstFunctionArgList *args, AstNode *block, int lineno) {

    AstFunctionDeclarationNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FUNCTION_DECLARATION_CONST;
    astNode->lineno = lineno;

    astNode->returnType = returnType;
    astNode->functionName = name;
    astNode->args = args;
    astNode->block = block;

    return (AstNode *) astNode;
}

AstNode * new_ast_function_call_node(char *name, AstFunctionArgList *args, int lineno) {

    AstFunctionCallNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FUNCTION_CALL_CONST;
    astNode->lineno = lineno;

    astNode->functionName = name;
    astNode->args = args;

    return (AstNode *) astNode;
}

AstFunctionArgList *ast_create_function_arg_list() {

    AstFunctionArgList *list = emalloc(sizeof(*list));

    list->argCount = 0;
    list->first = NULL;
    list->last = NULL;

    return list;
}

AstFunctionArgNode * ast_add_function_arg_declaration(AstFunctionArgList *list, int type, char* name) {

    AstFunctionArgNode *node = emalloc(sizeof(*node));

    node->symbolName = name;
    node->type = type;
    node->value = NULL;

    node->next = NULL;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    }

    else {
        list->last->next = node;
        list->last = node;
    }

    list->argCount++;

    return node;
}

AstFunctionArgNode * ast_add_function_arg_value(AstFunctionArgList *list, AstNode *value) {

    AstFunctionArgNode *node = emalloc(sizeof(*node));

    node->symbolName = NULL;
    node->value = value;
    node->next = NULL;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    }

    else {
        list->last->next = node;
        list->last = node;
    }

    list->argCount++;

    return node;

}

void ast_free_function_arg_list(AstFunctionArgList *list) {

    if(list == NULL)
        return;

    AstFunctionArgNode *iter = list->first;
    AstFunctionArgNode *next;

    while(iter != NULL) {

        next = iter->next;

        if(iter->value != NULL)
            free_ast_tree(iter->value);

        free(iter);

        iter = next;
    }

    free(list);
}

#include "astNodeFunctions.c"

int execute_ast_tree(AstNode *tree, SymbolTable st) {
    AstOpProcessorReturnNode *resultNode = execute_ast_node(tree, st);

    if(resultNode != NULL && resultNode->returnType == INT)
        return resultNode->value.intValue;

    return 0;
}

void free_ast_tree(AstNode *node) {

    if(node == NULL)
        return;

    int nodeType = node->nodeType;

    astNodeFunctions[AST_OP_POSITION(nodeType)].destroyer(node);
}