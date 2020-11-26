#include "astNodes.h"

#include <stdlib.h>
#include <stdio.h>

#include "utils/errorHandling.h"

AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right, char* filename, int lineno) {

    AstNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = nodeType;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->left = left;
    astNode->right = right;

    return astNode;
    }

AstNode * new_ast_symbol_reference_node(char *symbolName, char* filename, int lineno) {

    AstSymbolReferenceNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = ID;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;

    return (AstNode *) astNode;
}

AstNode * new_ast_if_node(AstNode * condition, AstNode * ifBranch, AstNode * elseBranch, char* filename, int lineno) {

    AstIfNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = IF;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->condition = condition;
    astNode->ifBranch = ifBranch;
    astNode->elseBranch = elseBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_while_node(int nodeType, AstNode *condition, AstNode *whileBranch, char* filename, int lineno) {

    if(nodeType != WHILE && nodeType != DO) {
        fprintf(stderr, "new_ast_while_node: Invalid type %d. Only valid %d (WHILE) and %d (DO)", nodeType, WHILE, DO);
        abort();
    }

    AstWhileNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = WHILE;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->condition = condition;
    astNode->whileBranch = whileBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_for_node(AstNode *firstAssignment, AstNode *condition, AstNode *lastAssignment, AstNode *forBranch, char* filename, int lineno) {

    AstForNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FOR;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->firstAssignment = firstAssignment;
    astNode->condition = condition;
    astNode->lastAssignment = lastAssignment;
    astNode->forBranch = forBranch;
    
    return (AstNode *) astNode;
}

AstNode * new_ast_declaration_node(int type, char *symbolName, AstNode *value, char* filename, int lineno) {

    AstDeclarationNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = DECLARATION;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->type = type;
    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_assignment_node(char *symbolName, AstNode *value, char* filename, int lineno) {

    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = '=';
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;
    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_inc_dec_assignment_node(int type, char *symbolName, char* filename, int lineno) {
    
    AstAssignmentNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = type;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;
    astNode->value = NULL;

    return (AstNode *) astNode;
}

AstNode * new_ast_set_property_node(char *symbolName, int propertyType, AstNode *value, char* filename, int lineno) {
    
    AstSetPropertyNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = SET_PROPERTY_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->propertyType = propertyType;   // BODY o NAME
    astNode->symbolName = symbolName;       // ID
    astNode->value = value;                 // <string>

    return (AstNode *) astNode;
}

AstNode * new_ast_set_named_property_node(char *symbolName, int propertyType, AstNode *propertyName, AstNode *value, char* filename, int lineno) {
    
    AstSetNamedPropertyNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = SET_NAMED_PROPERTY_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->propertyType = propertyType;   // ATTRIBUTE
    astNode->propertyName = propertyName;   // <string>
    astNode->symbolName = symbolName;       // ID
    astNode->value = value;                 // <string>

    return (AstNode *) astNode;
}

AstNode * new_ast_append_child_node(char *symbolName, AstNode *value, char* filename, int lineno) {
    
    AstAppendChildNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = APPEND_CHILD;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->symbolName = symbolName;       // ID
    astNode->value = value;                 // <tag>

    return (AstNode *) astNode;
}

AstNode * new_ast_get_property_node(char *symbolName, int propertyType, char* filename, int lineno) {
    
    AstSetPropertyNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = GET_PROPERTY_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->propertyType = propertyType;   // BODY o NAME
    astNode->symbolName = symbolName;       // ID

    return (AstNode *) astNode;
}

AstNode * new_ast_get_named_property_node(char *symbolName, int propertyType, AstNode *propertyName, char* filename, int lineno) {
    
    AstSetNamedPropertyNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = GET_NAMED_PROPERTY_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->propertyType = propertyType;   // ATTRIBUTE
    astNode->propertyName = propertyName;   // <string>
    astNode->symbolName = symbolName;       // ID

    return (AstNode *) astNode;
}

AstNode * new_ast_has_named_property_node(char *symbolName, int propertyType, AstNode *propertyName, char* filename, int lineno) {

    AstHasNamedPropertyNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = HAS_NAMED_PROPERTY_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->propertyType = propertyType;   // ATTRIBUTE
    astNode->propertyName = propertyName;   // <string>
    astNode->symbolName = symbolName;       // ID

    return (AstNode *) astNode;
}


AstNode * new_ast_int_node(int value, char* filename, int lineno) {

    AstIntNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = INT;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_string_node(char *value, char* filename, int lineno) {

    AstStringNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = STRING;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_tag_node(char *filename, int lineno){

    AstTagNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = TAG;
    astNode->filename = filename;
    astNode->lineno = lineno;

    return (AstNode *) astNode;

}

AstNode * new_ast_return_node(AstNode *value, char *filename, int lineno) {

    AstReturnNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = RETURN;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->value = value;

    return (AstNode *) astNode;
}

AstNode * new_ast_function_declaration_node(int returnType, char *name, AstFunctionArgList *args, AstNode *block, char* filename, int lineno) {

    AstFunctionDeclarationNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FUNCTION_DECLARATION_CONST;
    astNode->filename = filename;
    astNode->lineno = lineno;

    astNode->returnType = returnType;
    astNode->functionName = name;
    astNode->args = args;
    astNode->block = block;

    return (AstNode *) astNode;
}

AstNode * new_ast_function_call_node(char *name, AstFunctionArgList *args, char* filename, int lineno) {

    AstFunctionCallNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = FUNCTION_CALL_CONST;
    astNode->filename = filename;
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

#include "_astNodeFunctions.c"

Tag * execute_ast_tree(AstNode *tree, SymbolTable st) {

    debug_print("\n-- Starting Tree Excecution --\n");

    AstOpProcessorReturnNode *resultNode = execute_ast_node(tree, st);

    debug_print("\n-- Ending Tree Excecution --\n\n");

    Tag *returnVal = NULL;

    if(resultNode != NULL && resultNode->returnType == TAG)
        returnVal = resultNode->value.tagValue;
    
    if(resultNode != NULL)
        free(resultNode);

    return returnVal;
}

void free_ast_tree(AstNode *tree) {

    free_ast_node(tree);
}