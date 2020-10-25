
#include "astNodes.h"

static void * emalloc(size_t size);


SymbolNode * new_symbol_node(char *name, int type, int intValue, char *stringValue) {

    SymbolNode * symbolNode = emalloc(sizeof(*symbolNode));

    symbolNode->name = name;

    symbolNode->type = type;

    symbolNode->intValue;

    symbolNode->stringValue = stringValue;

    return symbolNode;
}


AstNode * new_ast_node(int nodeType, AstNode * left, AstNode * right) {

    AstNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = nodeType;

    astNode->left = left;
    astNode->right = right;

    return astNode;
    }

AstNode * new_ast_symbol_reference_node(SymbolNode * symbol) {

    AstSymbolReferenceNode * astNode = emalloc(sizeof(*astNode));

    astNode->nodeType = ID;

    astNode->symbol = symbol;

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

AstNode * new_ast_assignment_node(SymbolNode *symbol, AstNode *value) {

    AstAssignmentNode * astNode = emalloc (sizeof(*astNode));

    astNode->nodeType = '=';

    astNode->symbol = symbol;
    astNode->value = value;

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

static void * emalloc(size_t size) {

    void * pointer = malloc (size);

    if (!pointer) {

        eprintf ("Error: malloc(%u) failed!\n", size);
        abort();
    }

    return pointer;
}