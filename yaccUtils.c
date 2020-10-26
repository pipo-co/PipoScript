#include "yaccUtils.h"

void yyerror(char const *s) {

	fprintf(stderr, "Error in line %d: ", yylineno);
	fprintf(stderr, "\"%s\"\n",s);
	exit(3);
}

void initialize(void) {
	globalSt = symbol_table_create();
}

void finalize() {

    // ast_tree_free(astTree);
    symbol_table_free(globalSt);
}

void assign_symbol(SymbolNode *symbol, AstNode *valueTree) {

    SymbolValue value;
    // value = get_value(valueTree);

    // TODO: descablear - Tobi
        if(symbol->type == INT)
            value.intValue = 1;

        else if(symbol->type == STRING)
            value.stringValue = "hola";
    // Fin descablear

    if(symbol->type == INT) 
        symbol->value.intValue = value.intValue;

    else if(symbol->type == STRING)
        symbol->value.stringValue = value.stringValue;

    else
        yyerror("Invalid variable type assignment");

}

AstNode * create_inc_assignment_node(SymbolNode *symbol) {

    if(symbol->type != INT)
        yyerror("Invalid type for arithmetic operation");

    AstNode *newValue = new_ast_int_node(symbol->value.intValue + 1);

    return new_ast_assignment_node(symbol, newValue);
}

AstNode * create_dec_assignment_node(SymbolNode *symbol) {

    if(symbol->type != INT)
        yyerror("Invalid type for arithmetic operation");

    AstNode *newValue = new_ast_int_node(symbol->value.intValue - 1);

    return new_ast_assignment_node(symbol, newValue);
}