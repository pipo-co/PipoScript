#include "astNodes.h"

#define ASCII_TABLE_SIZE 128
#define AST_OP_COUNT (int)(ASCII_TABLE_SIZE + LAST - FIRST)

#define AST_OP_POSITION(op) (int)((op < ASCII_TABLE_SIZE)? op : ASCII_TABLE_SIZE + op - FIRST - 1)


typedef AstNode * (*AstNodeProcessor)(AstNode*, SymbolTable); 

static AstNodeProcessor nodeProcessors[AST_OP_COUNT];


static int ast_node_get_int_condition(AstNode *conditionNode);
static int ast_node_get_int(AstNode *intNode, char* message);


static AstNode * ast_if_node_processor(AstNode *node, SymbolTable st) {
    AstIfNode * ifNode = (AstIfNode*) node;

    printf("IF\n");

    if(ast_node_get_int_condition(execute_ast_tree(ifNode->condition, st)))
        execute_ast_tree(ifNode->ifBranch, st);
    
    else
        execute_ast_tree(ifNode->elseBranch, st);

    return NULL;
}

static AstNode * ast_while_node_processor(AstNode *node , SymbolTable st) {
    AstWhileNode * whileNode = (AstWhileNode*) node;

    printf("WHILE\n");

    while(ast_node_get_int_condition(execute_ast_tree(whileNode->condition, st)))
        execute_ast_tree(whileNode->whileBranch, st);

    return NULL;
}

static AstNode * ast_do_while_node_processor(AstNode *node, SymbolTable st) {
    AstWhileNode * doNode = (AstWhileNode*) node;

    printf("DO\n");

    do {
        execute_ast_tree(doNode->whileBranch, st);

    } while(ast_node_get_int_condition(execute_ast_tree(doNode->condition, st)));

    return NULL;
}

static AstNode * ast_for_node_processor(AstNode *node, SymbolTable st) {
    AstForNode * forNode = (AstForNode*) node;

    printf("FOR\n");

    execute_ast_tree(forNode->firstAssignment, st);

    while(ast_node_get_int_condition(execute_ast_tree(forNode->condition, st))) {

        execute_ast_tree(forNode->forBranch, st);
        execute_ast_tree(forNode->lastAssignment, st);
    }

    return NULL;
}

static AstNode * ast_declaration_node_processor(AstNode *node, SymbolTable st) {
    AstDeclarationNode * declarationNode = (AstDeclarationNode*) node;

    if(declarationNode->type != INT && declarationNode->type != STRING)
        print_and_abort("Invalid declaration type");

    SymbolNode *symbol = symbol_table_add(st, declarationNode->symbolName, declarationNode->type);

    if(declarationNode->value != NULL) {
        AstNode *valueNode = execute_ast_tree(declarationNode->value, st);

        if(valueNode->nodeType != symbol->type)
            print_and_abort("Declared and assigned types don't match");

        if(symbol->type == INT) {
            symbol->value.intValue = ((AstIntNode*)valueNode)->value;
            printf("int %s = %d;\n", symbol->name, symbol->value.intValue);
        }

        else {
            symbol->value.stringValue = ((AstStringNode*)valueNode)->value;
            printf("string %s = %s;\n", symbol->name, symbol->value.stringValue);
        }
    }
    else
        printf("%s %s;\n", (symbol->type == INT)? "int" : "string", symbol->name);

    return NULL;
}

static AstNode * ast_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);
    AstNode *value = execute_ast_tree(assignmentNode->value, st);

    if(symbol->type == INT) {
        
        if(value == NULL || value->nodeType != INT)
            print_and_abort("Tried to assign an invalid type to an int variable");

        printf("%s = %d;\n", symbol->name, ((AstIntNode*)value)->value);
        symbol->value.intValue = ((AstIntNode*)value)->value;
    }

    else if(symbol->type == STRING) {

        if(value == NULL || value->nodeType != STRING)
            print_and_abort("Tried to assign an invalid type to a string variable");

        // Dealloc previously allocated string if necessary
        if(symbol->value.stringValue != NULL)
            free(symbol->value.stringValue);

        printf("%s = %s;\n", symbol->name, ((AstStringNode*)value)->value);
        symbol->value.stringValue = ((AstStringNode*)value)->value;
    }

    else
        print_and_abort("Invalid data type assignment");

    return NULL;
}

static AstNode * ast_inc_dec_assignment_node_processor(AstNode *node, SymbolTable st) {
    AstAssignmentNode * assignmentNode = (AstAssignmentNode*) node;

    SymbolNode *symbol = symbol_table_get(st, assignmentNode->symbolName);
    
    if(symbol->type != INT)
        print_and_abort("++ and -- operations are only aplicable to ints");

    symbol->value.intValue++;

    return NULL;
}

static AstNode * ast_int_node_processor(AstNode *node, SymbolTable st) {
    AstIntNode * intNode = (AstIntNode*) node;

    printf("%d", intNode->value);

    return (AstNode*)intNode;
}

static AstNode * ast_string_node_processor(AstNode *node, SymbolTable st) {
    AstStringNode * stringNode = (AstStringNode*) node;

    printf("%s", stringNode->value);

    return (AstNode*)stringNode;
}

static AstNode * ast_symbol_reference_node_processor(AstNode *node, SymbolTable st) {
    AstSymbolReferenceNode * symbolRefNode = (AstSymbolReferenceNode*) node;

    printf("Symbol Reference\n");

    SymbolNode *symbol = symbol_table_get(st, symbolRefNode->symbolName);

    if(symbol->type == INT) {
        printf("%d", symbol->value.intValue);
        return new_ast_int_node(symbol->value.intValue);
    }

    else if(symbol->type == STRING) {
        printf("%s", symbol->value.stringValue);
        return new_ast_string_node(symbol->value.stringValue);
    }

    else
        print_and_abort("Variable with non existing type");

    return NULL;
}

static AstNode * ast_statement_list_node_processor(AstNode *node, SymbolTable st) {

    // Execute statement recursion to find first statement
    execute_ast_tree(node->left, st);

    printf("Statement\n");

    // Execute the corresponding statement for this iteration
    execute_ast_tree(node->right, st);

    return NULL;
}

// Binary and Unary Operators

static AstNode * ast_and_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of %% must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of && must be ints");

    printf("(%d && %d)", left, right);

    return new_ast_int_node(left && right);
}

static AstNode * ast_or_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of || must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of || must be ints");

    printf("(%d || %d)", left, right);

    return new_ast_int_node(left || right);
}

static AstNode * ast_gt_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of > must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of > must be ints");

    printf("(%d > %d)", left, right);

    return new_ast_int_node(left > right);
}

static AstNode * ast_lt_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of < must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of < must be ints");

    printf("(%d < %d)", left, right);

    return new_ast_int_node(left < right);
}

static AstNode * ast_ge_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of >= must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of >= must be ints");

    printf("(%d >= %d)", left, right);

    return new_ast_int_node(left >= right);
}

static AstNode * ast_le_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of <= must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of <= must be ints");

    printf("(%d <= %d)", left, right);

    return new_ast_int_node(left <= right);
}

static AstNode * ast_eq_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of == must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of == must be ints");

    printf("(%d == %d)", left, right);

    return new_ast_int_node(left == right);
}

static AstNode * ast_ne_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of != must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of != must be ints");

    printf("(%d != %d)", left, right);

    return new_ast_int_node(left != right);
}

static AstNode * ast_sum_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of + must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of + must be ints");

    printf("(%d + %d)", left, right);

    return new_ast_int_node(left + right);
}

static AstNode * ast_minus_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of - must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of - must be ints");

    printf("(%d - %d)", left, right);

    return new_ast_int_node(left - right);
}

static AstNode * ast_mult_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of * must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of * must be ints");

    printf("(%d * %d)", left, right);

    return new_ast_int_node(left * right);
}

static AstNode * ast_div_node_processor(AstNode *node, SymbolTable st) {

    int left = ast_node_get_int(execute_ast_tree(node->left, st), "Both operators of / must be ints");

    int right = ast_node_get_int(execute_ast_tree(node->right, st), "Both operators of / must be ints");

    printf("(%d / %d)", left, right);

    return new_ast_int_node(left / right);
}

static AstNode * ast_neg_node_processor(AstNode *node, SymbolTable st) {

    int val = ast_node_get_int(execute_ast_tree(node->left, st), "Operator of ! must be an int");

    printf("!(%d)", val);

    return new_ast_int_node(!val); 
}

static AstNode * ast_uminus_node_processor(AstNode *node, SymbolTable st) {

    int val = ast_node_get_int(execute_ast_tree(node->left, st), "You can only negate an int");

    printf("-(%d)", val);

    return new_ast_int_node(-val); 
}

void initialize_ast_node_functions() {

    nodeProcessors[AST_OP_POSITION(IF)] = ast_if_node_processor;

    nodeProcessors[AST_OP_POSITION(WHILE)] = ast_while_node_processor;

    nodeProcessors[AST_OP_POSITION(DO)] = ast_do_while_node_processor;

    nodeProcessors[AST_OP_POSITION(FOR)] = ast_for_node_processor;

    nodeProcessors[AST_OP_POSITION(DECLARATION)] = ast_declaration_node_processor;

    nodeProcessors[AST_OP_POSITION('=')] = ast_assignment_node_processor;

    nodeProcessors[AST_OP_POSITION(INC)] = ast_inc_dec_assignment_node_processor;

    nodeProcessors[AST_OP_POSITION(DEC)] = ast_inc_dec_assignment_node_processor;

    nodeProcessors[AST_OP_POSITION(INT)] = ast_int_node_processor;

    nodeProcessors[AST_OP_POSITION(STRING)] = ast_string_node_processor;

    nodeProcessors[AST_OP_POSITION(ID)] = ast_symbol_reference_node_processor;

    nodeProcessors[AST_OP_POSITION(STATEMENT_LIST_CONST)] = ast_statement_list_node_processor;

    nodeProcessors[AST_OP_POSITION(AND)] = ast_and_node_processor;

    nodeProcessors[AST_OP_POSITION(OR)] = ast_or_node_processor;

    nodeProcessors[AST_OP_POSITION('>')] = ast_gt_node_processor;

    nodeProcessors[AST_OP_POSITION('<')] = ast_lt_node_processor;

    nodeProcessors[AST_OP_POSITION(GE)] = ast_ge_node_processor;

    nodeProcessors[AST_OP_POSITION(LE)] = ast_le_node_processor;

    nodeProcessors[AST_OP_POSITION(EQ)] = ast_eq_node_processor;

    nodeProcessors[AST_OP_POSITION(NE)] = ast_ne_node_processor;

    nodeProcessors[AST_OP_POSITION('+')] = ast_sum_node_processor;

    nodeProcessors[AST_OP_POSITION('-')] = ast_minus_node_processor;

    nodeProcessors[AST_OP_POSITION('*')] = ast_mult_node_processor;

    nodeProcessors[AST_OP_POSITION('/')] = ast_div_node_processor;

    nodeProcessors[AST_OP_POSITION('!')] = ast_neg_node_processor;

    nodeProcessors[AST_OP_POSITION(UMINUS)] = ast_uminus_node_processor;
}

static int ast_node_get_int(AstNode *intNode, char* message) {

    if(intNode == NULL || intNode->nodeType != INT)
        print_and_abort(message);

    return ((AstIntNode*)intNode)->value;
}

static int ast_node_get_int_condition(AstNode *conditionNode) {
    return ast_node_get_int(conditionNode, "Condition must be present and an int");
}

