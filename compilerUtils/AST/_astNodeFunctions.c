#include "astNodes.h"

#include <stdlib.h>

#include "compilerUtils/functionSymbolTable/functionSymbolTable.h"
#include "compilerUtils/pipoUtils/pipoUtils.h"

#define ASCII_TABLE_SIZE 128
#define AST_OP_COUNT (int)(ASCII_TABLE_SIZE + LAST - FIRST)

#define AST_OP_POSITION(op) (int)((op < ASCII_TABLE_SIZE)? op : ASCII_TABLE_SIZE + op - FIRST - 1)

typedef struct AstOpProcessorReturnNode {
    int returnType;
    bool returnGenerated;

    SymbolValue value;
} AstOpProcessorReturnNode;

typedef AstOpProcessorReturnNode * (*AstNodeProcessor)(AstNode*, SymbolTable); 
typedef void (*AstNodeDestroyer)(AstNode*);

typedef struct AstOpFunctions {
    AstNodeProcessor processor;
    AstNodeDestroyer destroyer;
} AstOpFunctions;

static AstOpFunctions astNodeFunctions[AST_OP_COUNT];

// Main function
static AstOpProcessorReturnNode * execute_ast_node(AstNode *node, SymbolTable st);

// Return Value Auxiliary Functions
static AstOpProcessorReturnNode * ast_node_create_int_return_val(int val);
static AstOpProcessorReturnNode * ast_node_create_string_return_val(char *val);
static AstOpProcessorReturnNode * ast_node_create_tag_return_val(Tag *val);
static AstOpProcessorReturnNode * ast_node_create_void_return_val(void);
static int ast_node_get_int_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno);
static char * ast_node_get_string_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno);
static Tag * ast_node_get_tag_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno);


AstOpProcessorReturnNode * execute_ast_node(AstNode *node, SymbolTable st) {

    if(node == NULL)
        return NULL;

    int nodeType = node->nodeType;

    return astNodeFunctions[AST_OP_POSITION(nodeType)].processor(node, st);
}

static AstOpProcessorReturnNode * ast_if_node_processor(AstNode *node, SymbolTable st) {
    AstIfNode * ifNode = (AstIfNode*) node;

    fprintf(stderr, "IF\n");

    if(ast_node_get_int_return_val(execute_ast_node(ifNode->condition, st), "Missing if condition return value (int)", node->lineno))
        return execute_ast_node(ifNode->ifBranch, st);
    
    else
        return execute_ast_node(ifNode->elseBranch, st);
}

static void ast_if_node_destroyer(AstNode *node) {
    AstIfNode * ifNode = (AstIfNode*) node;

    free_ast_tree(ifNode->condition);
    free_ast_tree(ifNode->ifBranch);
    free_ast_tree(ifNode->elseBranch);

    free(ifNode);
}

static AstOpProcessorReturnNode * ast_while_node_processor(AstNode *node , SymbolTable st) {
    AstWhileNode * whileNode = (AstWhileNode*) node;

    fprintf(stderr, "WHILE\n");

    AstOpProcessorReturnNode *returnNode;

    while(ast_node_get_int_return_val(execute_ast_node(whileNode->condition, st), "Missing while condition return value (int)", node->lineno)) {
        returnNode = execute_ast_node(whileNode->whileBranch, st);

        if(returnNode != NULL) {

            if(returnNode->returnGenerated)
                return returnNode;

            free(returnNode);
        }
    }

    return NULL;
}

static void ast_while_node_destroyer(AstNode *node) {
    AstWhileNode * whileNode = (AstWhileNode*) node;

    free_ast_tree(whileNode->condition);
    free_ast_tree(whileNode->whileBranch);

    free(whileNode);
}

static AstOpProcessorReturnNode * ast_do_while_node_processor(AstNode *node, SymbolTable st) {
    AstWhileNode * doNode = (AstWhileNode*) node;

    fprintf(stderr, "DO\n");

    AstOpProcessorReturnNode *returnNode;

    do {
        returnNode = execute_ast_node(doNode->whileBranch, st);

        if(returnNode != NULL) {
            if(returnNode->returnGenerated)
                return returnNode;

            free(returnNode);
        }

    } while(ast_node_get_int_return_val(execute_ast_node(doNode->condition, st), "Missing do-while condition return value (int)", node->lineno));

    return NULL;
}

static AstOpProcessorReturnNode * ast_for_node_processor(AstNode *node, SymbolTable st) {
    AstForNode * forNode = (AstForNode*) node;

    fprintf(stderr, "FOR\n");

    AstOpProcessorReturnNode *returnNode;

    execute_ast_node(forNode->firstAssignment, st);

    while(ast_node_get_int_return_val(execute_ast_node(forNode->condition, st), "Missing for condition return value (int)", node->lineno)) {

        returnNode = execute_ast_node(forNode->forBranch, st);

        if(returnNode != NULL) {
            if(returnNode->returnGenerated)
                return returnNode;

            free(returnNode);
        }
        
        execute_ast_node(forNode->lastAssignment, st);
    }

    return NULL;
}

static void ast_for_node_destroyer(AstNode *node) {
    AstForNode * forNode = (AstForNode*) node;

    free_ast_tree(forNode->condition);
    free_ast_tree(forNode->firstAssignment);
    free_ast_tree(forNode->lastAssignment);
    free_ast_tree(forNode->forBranch);

    free(forNode);
}

static AstOpProcessorReturnNode * ast_declaration_node_processor(AstNode *node, SymbolTable st) {
    AstDeclarationNode * declarationNode = (AstDeclarationNode*) node;

    SymbolNode *symbol = symbol_table_add(st, declarationNode->symbolName, declarationNode->type);

    if(symbol == NULL)
        print_lineno_and_abort("Symbol already declared", node->lineno);
    

    if(declarationNode->value != NULL) {
        AstOpProcessorReturnNode *valueNode = execute_ast_node(declarationNode->value, st);

        if(valueNode->returnType != symbol->type){
            if(valueNode != NULL)
                free(valueNode);

            print_lineno_and_abort("Declared and assigned types don't match", node->lineno);
        }

        if(symbol->type == INT) {
            symbol->value.intValue = ast_node_get_int_return_val(valueNode, NULL, node->lineno);
            fprintf(stderr, "int %s = %d;\n", symbol->name, symbol->value.intValue);
        }

        else if(symbol->type == STRING){
            symbol->value.stringValue = ast_node_get_string_return_val(valueNode, NULL, node->lineno);
            fprintf(stderr, "string %s = %s;\n", symbol->name, symbol->value.stringValue);
        }
        
        else if(symbol->type == TAG){
            symbol->value.tagValue = ast_node_get_tag_return_val(valueNode, NULL, node->lineno);
            fprintf(stderr, "tag %s = {name: '%s', body: '%.10s', attrCount: %d};\n", symbol->name, symbol->value.tagValue->name, symbol->value.tagValue->body,symbol->value.tagValue->attributes->size);
        }

        else {
            if(valueNode != NULL)
                free(valueNode);

            print_lineno_and_abort("Invalid declaration type", node->lineno);
        }
    }
    else {

        if(symbol->type == TAG) {
            symbol->value.tagValue = new_tag();
        }

        char * type;

        switch (symbol->type) {
        case INT:
            type = "int"; break;
        case STRING:
            type = "string"; break;
        case TAG:
            type = "tag"; break;
        }

        fprintf(stderr, "%s %s;\n", type, symbol->name);
    }

    return NULL;
}

static void ast_declaration_node_destroyer(AstNode *node) {
    AstDeclarationNode * declarationNode = (AstDeclarationNode*) node;

    free_ast_tree(declarationNode->value);

    free(declarationNode);
}

static AstOpProcessorReturnNode * ast_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);

    if(symbol == NULL)
        print_lineno_and_abort("Variable wasn't previously declared", node->lineno);
    
    AstOpProcessorReturnNode *value = execute_ast_node(assignmentNode->value, st);

    if(value->returnType != symbol->type) {
        if(value != NULL)
            free(value);

        print_lineno_and_abort("Declared and assigned types don't match", node->lineno);
    }

    if(symbol->type == INT) {
        
        int intVal = ast_node_get_int_return_val(value, "Tried to assign an invalid type to an int variable", node->lineno);

        fprintf(stderr, "%s = %d;\n", symbol->name, intVal);
        symbol->value.intValue = intVal;
    }

    else if(symbol->type == STRING) {

        char* stringVal = ast_node_get_string_return_val(value, "Tried to assign an invalid type to a string variable", node->lineno);

        fprintf(stderr, "%s = %s;\n", symbol->name, stringVal);
        symbol->value.stringValue = stringVal;
    }

    else if(symbol->type == TAG) {

        Tag* tagVal = ast_node_get_tag_return_val(value, "Tried to assign an invalid type to a tag variable", node->lineno);
        
        fprintf(stderr, "tag %s = {name: '%s', body: '%.10s', attrCount: %d};\n", symbol->name, symbol->value.tagValue->name, symbol->value.tagValue->body,symbol->value.tagValue->attributes->size);
        symbol->value.tagValue = tagVal;
    }

    else {
        if(value != NULL)
            free(value);

        print_lineno_and_abort("Invalid data type assignment", node->lineno);
    }

    return NULL;
}

static void ast_assignment_node_destroyer(AstNode *node) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    free_ast_tree(assignmentNode->value);

    free(assignmentNode);
}

static AstOpProcessorReturnNode * ast_inc_dec_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);

    if(symbol == NULL)
        print_lineno_and_abort("Variable wasn't previously declared", node->lineno);
    
    if(symbol->type != INT)
        print_lineno_and_abort("++ and -- operations are only aplicable to ints", node->lineno);

    if(assignmentNode->nodeType == INC) {
        symbol->value.intValue++;
        fprintf(stderr, "%s++ => %s = %d;\n", symbol->name, symbol->name, symbol->value.intValue);
    }

    else if(assignmentNode->nodeType == DEC){
        symbol->value.intValue--;
        fprintf(stderr, "%s-- => %s = %d;\n", symbol->name, symbol->name, symbol->value.intValue);
    }

    else
        print_lineno_and_abort("Invalid inc/dec assignment operand", node->lineno);

    return NULL;
}

static AstOpProcessorReturnNode * ast_set_property_node_processor(AstNode *node, SymbolTable st) {
    
    AstSetPropertyNode *setPropertyNode = (AstSetPropertyNode*) node;

    SymbolNode *symbol = symbol_table_get(st, setPropertyNode->symbolName);

    if(symbol == NULL)
        print_lineno_and_abort("Variable wasn't previously declared", node->lineno);
    
    if(symbol->type != TAG)
        print_lineno_and_abort("Set property operations are only aplicable to tags", node->lineno);
    
    char * stringValue = ast_node_get_string_return_val(execute_ast_node(setPropertyNode->value, st), "Type mismatch. Set property expects a string.", node->lineno);

    if(setPropertyNode->propertyType == BODY) {
        symbol->value.tagValue->body = stringValue;
        fprintf(stderr, "set body from %s = %s;\n", symbol->name, stringValue);
    }

    else if(setPropertyNode->propertyType == NAME) {
        symbol->value.tagValue->name = stringValue;
        fprintf(stderr, "set name from %s = %s;\n", symbol->name, stringValue);
    }

    else
        print_lineno_and_abort("Invalid property on set operation. Expecting body or name.", node->lineno);

    return NULL;
}

static void ast_set_property_node_destroyer(AstNode *node) {
    AstSetPropertyNode * setPropertyNode = (AstSetPropertyNode*) node;

    free_ast_tree(setPropertyNode->value);

    free(setPropertyNode);
}

static AstOpProcessorReturnNode * ast_set_named_property_node_processor(AstNode *node, SymbolTable st) {

    AstSetNamedPropertyNode *setNamedPropertyNode = (AstSetNamedPropertyNode*) node;

    SymbolNode *symbol = symbol_table_get(st, setNamedPropertyNode->symbolName);

    if(symbol == NULL)
        print_lineno_and_abort("Variable wasn't previously declared", node->lineno);
    
    if(symbol->type != TAG)
        print_lineno_and_abort("Set property operations are only aplicable to tags", node->lineno);
    
    char * stringValue = ast_node_get_string_return_val(execute_ast_node(setNamedPropertyNode->value, st), "Type mismatch. Set property expects a string.", node->lineno);

    if(setNamedPropertyNode->propertyType == ATTRIBUTE) {
        
        if(put_attribute(symbol->value.tagValue, setNamedPropertyNode->propertyName, stringValue))
            print_and_abort("Error adding attribute. Problem on internal hashing.");

        fprintf(stderr, "set attribute %s from %s = %s;\n", setNamedPropertyNode->propertyName, symbol->name, stringValue);
    }

    else
        print_lineno_and_abort("Invalid property on set operation. Expecting attribute.", node->lineno);

    return NULL;

}

static void ast_set_named_property_node_destroyer(AstNode *node) {
    AstSetNamedPropertyNode * setNamedPropertyNode = (AstSetNamedPropertyNode*) node;

    free_ast_tree(setNamedPropertyNode->value);

    free(setNamedPropertyNode);
}

static AstOpProcessorReturnNode * ast_int_node_processor(AstNode *node, SymbolTable st) {
    AstIntNode * intNode = (AstIntNode*) node;

    // fprintf(stderr, "%d", intNode->value);

    return ast_node_create_int_return_val(intNode->value);
}

static void ast_int_node_destroyer(AstNode *node) {
    AstIntNode * intNode = (AstIntNode*) node;

    free(intNode);
}

static AstOpProcessorReturnNode * ast_string_node_processor(AstNode *node, SymbolTable st) {
    AstStringNode * stringNode = (AstStringNode*) node;

    // fprintf(stderr, "%s", stringNode->value);

    return ast_node_create_string_return_val(stringNode->value);
}

static void ast_string_node_destroyer(AstNode *node) {
    AstStringNode * stringNode = (AstStringNode*) node;

    free(stringNode);
}

static AstOpProcessorReturnNode * ast_symbol_reference_node_processor(AstNode *node, SymbolTable st) {
    AstSymbolReferenceNode * symbolRefNode = (AstSymbolReferenceNode*) node;

    SymbolNode *symbol = symbol_table_get(st, symbolRefNode->symbolName);

    if(symbol == NULL)
        print_lineno_and_abort("Variable wasn't previously declared", node->lineno);

    if(symbol->type == INT) {
        fprintf(stderr, "Symbol Dereference %s (%d)\n", symbol->name, symbol->value.intValue);

        return ast_node_create_int_return_val(symbol->value.intValue);
    }

    else if(symbol->type == STRING) {
        fprintf(stderr, "Symbol Dereference %s (%s)\n", symbol->name, symbol->value.stringValue);

        return ast_node_create_string_return_val(symbol->value.stringValue);
    }

    else if(symbol->type == TAG) {
        fprintf(stderr, "Symbol Dereference %s ({name: '%s', body: '%.10s', attrCount: %d})\n", symbol->name,  symbol->value.tagValue->name, symbol->value.tagValue->body,symbol->value.tagValue->attributes->size);

        return ast_node_create_tag_return_val(symbol->value.tagValue);
    }

    else
        print_lineno_and_abort("Variable with non existing type", node->lineno);

    return NULL;
}

static void ast_symbol_reference_node_destroyer(AstNode *node) {
    AstSymbolReferenceNode * symbolRefNode = (AstSymbolReferenceNode*) node;

    free(symbolRefNode);
}

static AstOpProcessorReturnNode * ast_return_node_processor(AstNode *node, SymbolTable st) {
    AstReturnNode * returnNode = (AstReturnNode*) node;

    AstOpProcessorReturnNode *returnValueNode;

    if(returnNode->value == NULL) {
        returnValueNode = ast_node_create_void_return_val();
        returnValueNode->returnGenerated = true;

        fprintf(stderr, "return;\n");
        return returnValueNode;
    }

    returnValueNode = execute_ast_node(returnNode->value, st);
    returnValueNode->returnGenerated = true;

    // Print Log
    if(returnValueNode->returnType == VOID)
        fprintf(stderr, "return;\n");

    else if(returnValueNode->returnType == INT)
        fprintf(stderr, "return %d;\n", returnValueNode->value.intValue);

    else if(returnValueNode->returnType == STRING)
        fprintf(stderr, "return %s;\n", returnValueNode->value.stringValue);

    else
        fprintf(stderr, "return ???; [ERROR: return value isn't defined]\n");

    return returnValueNode;
}

static void ast_return_node_destroyer(AstNode *node) {
    AstReturnNode * returnNode = (AstReturnNode*) node;

    free_ast_tree(returnNode->value);

    free(returnNode);
}

static void ast_function_declaration_node_destroyer(AstNode *node) {
    AstFunctionDeclarationNode * declarationNode = (AstFunctionDeclarationNode*) node;

    ast_free_function_arg_list(declarationNode->args);

    free_ast_tree(declarationNode->block);

    free(declarationNode);
}

static AstOpProcessorReturnNode * ast_function_call_node_processor(AstNode *node, SymbolTable st) {
    AstFunctionCallNode * callNode = (AstFunctionCallNode*) node;

    AstFunctionDeclarationNode *declarationNode = function_symbol_table_get(callNode->functionName);

    if(declarationNode == NULL)
        print_lineno_and_abort("Function wasn't previously declared", node->lineno);

    // Both declaration and call have the same number of arguments (NULL == no arguments)
    if(declarationNode->args == NULL) {
        if(callNode->args != NULL)
            print_lineno_and_abort("Function call with invalid argument count", node->lineno);
    }

    else if(callNode->args == NULL)
        print_lineno_and_abort("Function call with invalid argument count", node->lineno);

    else if(declarationNode->args->argCount != callNode->args->argCount)
        print_lineno_and_abort("Function call with invalid argument count", node->lineno);


    SymbolTable functionST = symbol_table_create();

    if(declarationNode->args != NULL) {

        AstFunctionArgNode *iterCall = callNode->args->first;
        AstFunctionArgNode *iterDecl = declarationNode->args->first;

        SymbolValue value;

        for(int i = 0; i < declarationNode->args->argCount; i++) {

            if(iterDecl->type == INT)
                value.intValue = ast_node_get_int_return_val(execute_ast_node(iterCall->value, st), "Type mismatch on function arguments. Was expecting an int.", node->lineno);
            
            else if(iterDecl->type == STRING)
                value.stringValue = ast_node_get_string_return_val(execute_ast_node(iterCall->value, st), "Type mismatch on function arguments. Was expecting a string.", node->lineno);

            else if(iterDecl->type == TAG)
                value.tagValue = ast_node_get_tag_return_val(execute_ast_node(iterCall->value, st), "Type mismatch on function arguments. Was expecting a tag.", node->lineno);

            else
                print_lineno_and_abort("Invalid function argument type", node->lineno);

            SymbolNode *symbolNode = symbol_table_add(functionST, iterDecl->symbolName, iterDecl->type);

            if(declarationNode == NULL)
                print_lineno_and_abort("There are duplicated argument names in functions", node->lineno);

            symbolNode->value = value;

            iterCall = iterCall->next;
            iterDecl = iterDecl->next;
        }

        // Assert
        if(iterCall != NULL || iterDecl != NULL)
            print_lineno_and_abort("IterCall or IterDecl weren't null after parsing arguments (assert null)", node->lineno);

    }

    fprintf(stderr, "\nStart Function %s {\n", callNode->functionName);

    AstOpProcessorReturnNode *returnNode = execute_ast_node(declarationNode->block, functionST);

    fprintf(stderr, "} End Function %s\n", callNode->functionName);

    symbol_table_free(functionST);

    if((returnNode == NULL || !returnNode->returnGenerated) && declarationNode->returnType == VOID) {

        if(returnNode != NULL)
            free(returnNode);

        return ast_node_create_void_return_val();
    }
    
    if(returnNode == NULL || !returnNode->returnGenerated || returnNode->returnType != declarationNode->returnType) {

        if(returnNode != NULL)
            free(returnNode);

        print_lineno_and_abort("Function return type and actual return value don't match", node->lineno);
    }

    returnNode->returnGenerated = false;
    return returnNode;
}

static void ast_function_call_node_destroyer(AstNode *node) {
    AstFunctionCallNode * callNode = (AstFunctionCallNode*) node;

    ast_free_function_arg_list(callNode->args);

    free(callNode);
}

static AstOpProcessorReturnNode * ast_statement_list_node_processor(AstNode *node, SymbolTable st) {

    // Execute statement recursion to find first statement
    AstOpProcessorReturnNode *returnNode = execute_ast_node(node->left, st);

    if(returnNode != NULL) {
        if(returnNode->returnGenerated)
            return returnNode;
        
        free(returnNode);
    }

    // Execute the corresponding statement for this iteration
    return execute_ast_node(node->right, st);
}

// Binary and Unary Operators

static AstOpProcessorReturnNode * ast_and_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of %% must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of && must be ints", node->lineno);

    fprintf(stderr, "(%d && %d)", left, right);

    return ast_node_create_int_return_val(left && right);
}

static AstOpProcessorReturnNode * ast_or_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of || must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of || must be ints", node->lineno);

    fprintf(stderr, "(%d || %d)", left, right);

    return ast_node_create_int_return_val(left || right);
}

static AstOpProcessorReturnNode * ast_gt_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of > must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of > must be ints", node->lineno);

    fprintf(stderr, "(%d > %d)", left, right);

    return ast_node_create_int_return_val(left > right);
}

static AstOpProcessorReturnNode * ast_lt_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of < must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of < must be ints", node->lineno);

    fprintf(stderr, "(%d < %d)", left, right);

    return ast_node_create_int_return_val(left < right);
}

static AstOpProcessorReturnNode * ast_ge_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of >= must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of >= must be ints", node->lineno);

    fprintf(stderr, "(%d >= %d)", left, right);

    return ast_node_create_int_return_val(left >= right);
}

static AstOpProcessorReturnNode * ast_le_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of <= must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of <= must be ints", node->lineno);

    fprintf(stderr, "(%d <= %d)", left, right);

    return ast_node_create_int_return_val(left <= right);
}

static AstOpProcessorReturnNode * ast_eq_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of == must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of == must be ints", node->lineno);

    fprintf(stderr, "(%d == %d)", left, right);

    return ast_node_create_int_return_val(left == right);
}

static AstOpProcessorReturnNode * ast_ne_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of != must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of != must be ints", node->lineno);

    fprintf(stderr, "(%d != %d)", left, right);

    return ast_node_create_int_return_val(left != right);
}

static AstOpProcessorReturnNode * ast_sum_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of + must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of + must be ints", node->lineno);

    fprintf(stderr, "(%d + %d)", left, right);

    return ast_node_create_int_return_val(left + right);
}

static AstOpProcessorReturnNode * ast_minus_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of - must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of - must be ints", node->lineno);

    fprintf(stderr, "(%d - %d)", left, right);

    return ast_node_create_int_return_val(left - right);
}

static AstOpProcessorReturnNode * ast_mult_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of * must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of * must be ints", node->lineno);

    fprintf(stderr, "(%d * %d)", left, right);

    return ast_node_create_int_return_val(left * right);
}

static AstOpProcessorReturnNode * ast_div_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Both operators of / must be ints", node->lineno);

    int right = ast_node_get_int_return_val(execute_ast_node(node->right, st), "Both operators of / must be ints", node->lineno);

    fprintf(stderr, "(%d / %d)", left, right);

    return ast_node_create_int_return_val(left / right);
}

static AstOpProcessorReturnNode * ast_neg_node_processor(AstNode *node, SymbolTable st) {

    int val = ast_node_get_int_return_val(execute_ast_node(node->left, st), "Operator of ! must be an int", node->lineno);

    fprintf(stderr, "!(%d)", val);

    return ast_node_create_int_return_val(!val); 
}

static AstOpProcessorReturnNode * ast_uminus_node_processor(AstNode *node, SymbolTable st) {

    int val = ast_node_get_int_return_val(execute_ast_node(node->left, st), "You can only negate an int", node->lineno);

    fprintf(stderr, "-(%d)", val);

    return ast_node_create_int_return_val(-val); 
}

static void ast_node_destroyer(AstNode *node) {

    free_ast_tree(node->left);
    free_ast_tree(node->right);

    free(node);
}

// Export Node Operation Functions
void initialize_ast_node_functions() {

    astNodeFunctions[AST_OP_POSITION(IF)].processor = ast_if_node_processor;
    astNodeFunctions[AST_OP_POSITION(IF)].destroyer = ast_if_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(WHILE)].processor = ast_while_node_processor;
    astNodeFunctions[AST_OP_POSITION(WHILE)].destroyer = ast_while_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(DO)].processor = ast_do_while_node_processor;
    astNodeFunctions[AST_OP_POSITION(DO)].destroyer = ast_while_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(FOR)].processor = ast_for_node_processor;
    astNodeFunctions[AST_OP_POSITION(FOR)].destroyer = ast_for_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(DECLARATION)].processor = ast_declaration_node_processor;
    astNodeFunctions[AST_OP_POSITION(DECLARATION)].destroyer = ast_declaration_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('=')].processor = ast_assignment_node_processor;
    astNodeFunctions[AST_OP_POSITION('=')].destroyer = ast_assignment_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(INC)].processor = ast_inc_dec_assignment_node_processor;
    astNodeFunctions[AST_OP_POSITION(INC)].destroyer = ast_assignment_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(DEC)].processor = ast_inc_dec_assignment_node_processor;
    astNodeFunctions[AST_OP_POSITION(DEC)].destroyer = ast_assignment_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(SET_PROPERTY_CONST)].processor = ast_set_property_node_processor;
    astNodeFunctions[AST_OP_POSITION(SET_PROPERTY_CONST)].destroyer = ast_set_property_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(SET_NAMED_PROPERTY_CONST)].processor = ast_set_named_property_node_processor;
    astNodeFunctions[AST_OP_POSITION(SET_NAMED_PROPERTY_CONST)].destroyer = ast_set_named_property_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(INT)].processor = ast_int_node_processor;
    astNodeFunctions[AST_OP_POSITION(INT)].destroyer = ast_int_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(STRING)].processor = ast_string_node_processor;
    astNodeFunctions[AST_OP_POSITION(STRING)].destroyer = ast_string_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(ID)].processor = ast_symbol_reference_node_processor;
    astNodeFunctions[AST_OP_POSITION(ID)].destroyer = ast_symbol_reference_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(RETURN)].processor = ast_return_node_processor;
    astNodeFunctions[AST_OP_POSITION(RETURN)].destroyer = ast_return_node_destroyer;

    // Doesn't have processor
    astNodeFunctions[AST_OP_POSITION(FUNCTION_DECLARATION_CONST)].destroyer = ast_function_declaration_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(FUNCTION_CALL_CONST)].processor = ast_function_call_node_processor;
    astNodeFunctions[AST_OP_POSITION(FUNCTION_CALL_CONST)].destroyer = ast_function_call_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(STATEMENT_LIST_CONST)].processor = ast_statement_list_node_processor;
    astNodeFunctions[AST_OP_POSITION(STATEMENT_LIST_CONST)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(AND)].processor = ast_and_node_processor;
    astNodeFunctions[AST_OP_POSITION(AND)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(OR)].processor = ast_or_node_processor;
    astNodeFunctions[AST_OP_POSITION(OR)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('>')].processor = ast_gt_node_processor;
    astNodeFunctions[AST_OP_POSITION('>')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('<')].processor = ast_lt_node_processor;
    astNodeFunctions[AST_OP_POSITION('<')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(GE)].processor = ast_ge_node_processor;
    astNodeFunctions[AST_OP_POSITION(GE)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(LE)].processor = ast_le_node_processor;
    astNodeFunctions[AST_OP_POSITION(LE)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(EQ)].processor = ast_eq_node_processor;
    astNodeFunctions[AST_OP_POSITION(EQ)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(NE)].processor = ast_ne_node_processor;
    astNodeFunctions[AST_OP_POSITION(NE)].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('+')].processor = ast_sum_node_processor;
    astNodeFunctions[AST_OP_POSITION('+')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('-')].processor = ast_minus_node_processor;
    astNodeFunctions[AST_OP_POSITION('-')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('*')].processor = ast_mult_node_processor;
    astNodeFunctions[AST_OP_POSITION('*')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('/')].processor = ast_div_node_processor;
    astNodeFunctions[AST_OP_POSITION('/')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION('!')].processor = ast_neg_node_processor;
    astNodeFunctions[AST_OP_POSITION('!')].destroyer = ast_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(UMINUS)].processor = ast_uminus_node_processor;
    astNodeFunctions[AST_OP_POSITION(UMINUS)].destroyer = ast_node_destroyer;
}

static int ast_node_get_int_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno) {

    if(returnVal == NULL || returnVal->returnType != INT)
        print_lineno_and_abort((message != NULL)? message : "Error returning int. An int return value was needed and wasn't provided", lineno);

    int intVal = returnVal->value.intValue;

    free(returnVal);

    return intVal;
}

static char * ast_node_get_string_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno) {

    if(returnVal == NULL || returnVal->returnType != STRING)
        print_lineno_and_abort((message != NULL)? message : "Error returning string. A string return value was needed and wasn't provided", lineno);

    char* stringVal = returnVal->value.stringValue;

    free(returnVal);

    return stringVal;
}

static Tag * ast_node_get_tag_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno) {

    if(returnVal == NULL || returnVal->returnType != TAG)
        print_lineno_and_abort((message != NULL)? message : "Error returning tag. A tag return value was needed and wasn't provided", lineno);

    Tag *tagVal = returnVal->value.tagValue;

    free(returnVal);

    return tagVal;
}

static AstOpProcessorReturnNode * ast_node_create_int_return_val(int val) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = INT;
    ret->value.intValue = val;
    ret->returnGenerated = false;

    return ret;
}

static AstOpProcessorReturnNode * ast_node_create_string_return_val(char *val) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = STRING;
    ret->value.stringValue = val;
    ret->returnGenerated = false;

    return ret;
}

static AstOpProcessorReturnNode * ast_node_create_tag_return_val(Tag *val) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = TAG;
    ret->value.tagValue = val;
    ret->returnGenerated = false;

    return ret;
}

static AstOpProcessorReturnNode * ast_node_create_void_return_val(void) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = VOID;
    ret->returnGenerated = false;

    return ret;
}
