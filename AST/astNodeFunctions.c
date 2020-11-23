#include "astNodes.h"

#define ASCII_TABLE_SIZE 128
#define AST_OP_COUNT (int)(ASCII_TABLE_SIZE + LAST - FIRST)

#define AST_OP_POSITION(op) (int)((op < ASCII_TABLE_SIZE)? op : ASCII_TABLE_SIZE + op - FIRST - 1)

typedef struct AstOpProcessorReturnNode {
    int returnType;

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
static AstOpProcessorReturnNode * ast_node_create_int_return_val(int val) ;
static AstOpProcessorReturnNode * ast_node_create_string_return_val(char *val);
static int ast_node_get_int_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno);
static char * ast_node_get_string_return_val(AstOpProcessorReturnNode *returnVal, char* message, int lineno);


static AstOpProcessorReturnNode * execute_ast_node(AstNode *node, SymbolTable st) {

    if(node == NULL)
        return NULL;

    int nodeType = node->nodeType;

    return astNodeFunctions[AST_OP_POSITION(nodeType)].processor(node, st);
}

static AstOpProcessorReturnNode * ast_if_node_processor(AstNode *node, SymbolTable st) {
    AstIfNode * ifNode = (AstIfNode*) node;

    fprintf(stderr, "IF\n");

    if(ast_node_get_int_return_val(execute_ast_node(ifNode->condition, st), "Missing if condition return value (int)", node->lineno))
        execute_ast_node(ifNode->ifBranch, st);
    
    else
        execute_ast_node(ifNode->elseBranch, st);

    return NULL;
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

    while(ast_node_get_int_return_val(execute_ast_node(whileNode->condition, st), "Missing while condition return value (int)", node->lineno))
        execute_ast_node(whileNode->whileBranch, st);

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

    do {
        execute_ast_node(doNode->whileBranch, st);

    } while(ast_node_get_int_return_val(execute_ast_node(doNode->condition, st), "Missing do-while condition return value (int)", node->lineno));

    return NULL;
}

static AstOpProcessorReturnNode * ast_for_node_processor(AstNode *node, SymbolTable st) {
    AstForNode * forNode = (AstForNode*) node;

    fprintf(stderr, "FOR\n");

    execute_ast_node(forNode->firstAssignment, st);

    while(ast_node_get_int_return_val(execute_ast_node(forNode->condition, st), "Missing for condition return value (int)", node->lineno)) {

        execute_ast_node(forNode->forBranch, st);
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

    if(declarationNode->value != NULL) {
        AstOpProcessorReturnNode *valueNode = execute_ast_node(declarationNode->value, st);

        if(valueNode->returnType != symbol->type)
            print_lineno_and_abort("Declared and assigned types don't match", node->lineno);

        if(symbol->type == INT) {
            symbol->value.intValue = ast_node_get_int_return_val(valueNode, NULL, node->lineno);
            fprintf(stderr, "int %s = %d;\n", symbol->name, symbol->value.intValue);
        }

        else if(symbol->type == STRING){
            symbol->value.stringValue = ast_node_get_string_return_val(valueNode, NULL, node->lineno);
            fprintf(stderr, "string %s = %s;\n", symbol->name, symbol->value.stringValue);
        }

        else
            print_lineno_and_abort("Invalid declaration type", node->lineno);
    }
    else
        fprintf(stderr, "%s %s;\n", (symbol->type == INT)? "int" : "string", symbol->name);

    return NULL;
}

static void ast_declaration_node_destroyer(AstNode *node) {
    AstDeclarationNode * declarationNode = (AstDeclarationNode*) node;

    free(declarationNode->symbolName);
    free_ast_tree(declarationNode->value);

    free(declarationNode);
}

static AstOpProcessorReturnNode * ast_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);
    AstOpProcessorReturnNode *value = execute_ast_node(assignmentNode->value, st);

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

    else
        print_lineno_and_abort("Invalid data type assignment", node->lineno);

    return NULL;
}

static void ast_assignment_node_destroyer(AstNode *node) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    free_ast_tree(assignmentNode->value);
    free(assignmentNode->symbolName);

    free(assignmentNode);
}

static AstOpProcessorReturnNode * ast_inc_dec_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);
    
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

    free(stringNode->value);

    free(stringNode);
}

static AstOpProcessorReturnNode * ast_symbol_reference_node_processor(AstNode *node, SymbolTable st) {
    AstSymbolReferenceNode * symbolRefNode = (AstSymbolReferenceNode*) node;

    SymbolNode *symbol = symbol_table_get(st, symbolRefNode->symbolName);

    if(symbol->type == INT) {
        fprintf(stderr, "Symbol Dereference %s (%d)\n", symbol->name, symbol->value.intValue);

        return ast_node_create_int_return_val(symbol->value.intValue);
    }

    else if(symbol->type == STRING) {
        fprintf(stderr, "Symbol Dereference %s (%s)\n", symbol->name, symbol->value.stringValue);

        return ast_node_create_string_return_val(symbol->value.stringValue);
    }

    else
        print_lineno_and_abort("Variable with non existing type", node->lineno);

    return NULL;
}

static void ast_symbol_reference_node_destroyer(AstNode *node) {
    AstSymbolReferenceNode * symbolRefNode = (AstSymbolReferenceNode*) node;

    free(symbolRefNode->symbolName);

    free(symbolRefNode);
}

static AstOpProcessorReturnNode * ast_function_declaration_node_destroyer(AstNode *node, SymbolTable st) {
    AstFunctionDeclarationNode * declarationNode = (AstSymbolReferenceNode*) node;

    free(declarationNode->functionName);

    ast_free_function_arg_list(declarationNode->args);

    free_ast_tree(declarationNode->block);
}

static AstOpProcessorReturnNode * ast_function_call_node_processor(AstNode *node, SymbolTable st) {
    AstFunctionCallNode * callNode = (AstSymbolReferenceNode*) node;

    AstFunctionDeclarationNode *declarationNode; // Function Symbol Table
    // AstFunctionDeclarationNode *declarationNode = function_symbol_table_get(callNode->functionName);

    if(declarationNode->args->argCount != callNode->args->argCount)
        print_lineno_and_abort("Function call with invalid argument count", node->lineno);

    SymbolTable functionST = symbol_table_create();

    AstFunctionArgNode *iterCall = callNode->args->first;
    AstFunctionArgNode *iterDecl = declarationNode->args->first;

    SymbolValue value;

    for(int i = 0; i < declarationNode->args->argCount; i++) {

        if(iterDecl->type == INT)
            value.intValue = ast_node_get_int_return_val(execute_ast_node(iterCall->value, st), "Type mismatch on function arguments. Was expecting an int.", node->lineno);
        

        else if(iterDecl->type == STRING)
            value.intValue = ast_node_get_string_return_val(execute_ast_node(iterCall->value, st), "Type mismatch on function arguments. Was expecting a string.", node->lineno);

        else
            print_lineno_and_abort("Invalid function argument type", node->lineno);

        SymbolNode *symbolNode = symbol_table_add(functionST, iterDecl->symbolName, iterDecl->type);
        symbolNode->value = value;

        iterCall = iterCall->next;
        iterDecl = iterDecl->next;
    }

    // Assert
    if(iterCall != NULL || iterDecl != NULL)
        print_lineno_and_abort("IterCall or IterDecl weren't null after parsing arguments (assert null)", node->lineno);

    AstOpProcessorReturnNode *returnNode = execute_ast_node(declarationNode->block, functionST);

    symbol_table_free(functionST);

    if(returnNode->returnType != declarationNode->returnType)
        print_lineno_and_abort("Function return type and actual return value don't match", node->lineno);

    return returnNode;
}

static AstOpProcessorReturnNode * ast_function_call_node_destroyer(AstNode *node, SymbolTable st) {
    AstFunctionCallNode * callNode = (AstSymbolReferenceNode*) node;

    free(callNode->functionName);

    ast_free_function_arg_list(callNode->args);
}

static AstOpProcessorReturnNode * ast_statement_list_node_processor(AstNode *node, SymbolTable st) {

    // Execute statement recursion to find first statement
    execute_ast_node(node->left, st);

    // Execute the corresponding statement for this iteration
    execute_ast_node(node->right, st);

    return NULL;
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

    astNodeFunctions[AST_OP_POSITION(INT)].processor = ast_int_node_processor;
    astNodeFunctions[AST_OP_POSITION(INT)].destroyer = ast_int_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(STRING)].processor = ast_string_node_processor;
    astNodeFunctions[AST_OP_POSITION(STRING)].destroyer = ast_string_node_destroyer;

    astNodeFunctions[AST_OP_POSITION(ID)].processor = ast_symbol_reference_node_processor;
    astNodeFunctions[AST_OP_POSITION(ID)].destroyer = ast_symbol_reference_node_destroyer;

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

static AstOpProcessorReturnNode * ast_node_create_int_return_val(int val) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = INT;
    ret->value.intValue = val;

    return ret;
}

static AstOpProcessorReturnNode * ast_node_create_string_return_val(char *val) {
    AstOpProcessorReturnNode *ret = emalloc(sizeof(*ret));

    ret->returnType = STRING;
    ret->value.stringValue = val;

    return ret;
}
