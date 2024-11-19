#include <stdbool.h>

#include "structs/ast.h"

#include "parser/semantic_checks.h"

int semantic_error_code = -1;


int SemCheck_Run(ASTNode* program) {
    if (!AddFunctionsToSymtable(program->right)) {
        // Unexpected problem. Like failure to add functions to symtable
    }

    semantic_error_code = SemCheck_FunctionList(program->right);
    if (semantic_error_code != 0) {
        return 2;
    }
    return 0;
}

/// @brief Must be done so that we can call functions before they are declated in text.
int AddFunctionsToSymtable(ASTNode* node) {
    return 0;
}

int SemCheck_FunctionList(ASTNode* node) {
    // If there isn't a main function in the symtable, return [error_code]

    // If main function returns unexpected type or has unexpected arguments, return [error_code]

    // For each function, run SemCheck_Function

    return 0;
}


int SemCheck_Function(ASTNode* node) {
    // If function is already declared return [error_code]
    
    // Function returns void but has a return statement with a value, return [error_code]
    // Function doesn't return void but doesn't have a return statement with a value, return [error_code]
    // If return statement returns a different type than what's expected, return [error_code]

    // Run SemCheck_Statement for each statement in the function body
    // - SemCheck_Block is unsuitable for this, because it doesn't check for return statements

    return 0;
}


int SemCheck_Statement(ASTNode* node) {
    // If declaration, run SemCheck_Declaration
    // If assignment, run SemCheck_Assignment
    // If an if statement, run SemCheck_ConditionalBlock
    // If a new block, run SemCheck_Block

    return 0;
}

int SemCheck_Declaration(ASTNode* node) {
    // Get var name from properties
    // return [error_code] if already declared in symtable

    // Get type from left
    // If type is not specified and it's not possible to infer it, return [error_code]

    // TODO: If an assignment is next, run SemCheck_Assignment with extra context?

    return 0;
}

int SemCheck_Assignment(ASTNode* node) {
    // Get var name from properties
    // return [error_code] if not already declared in symtable
    // return [error_code] if const

    // Run SemCheck_Expression on new value (right)
    // If variable'a and new value's types are different, return [error_code]

    // TODO: Assignment of strings must be done like: []u8 a = ifj.string("abc");
    // keep that in mind

    return 0;
}

int SemCheck_Expression(ASTNode* node) {
    // Can be a binary operation or a single value

    // If single value, return true

    // If a function call, run SemCheck_FuncCall

    // If binary operation,
    //   Run SemCheck_Expression on both perands
    //   If types are different, return [error_code]
    //   If operation is undefined on these types, return [error_code]

    return 0;
}

int SemCheck_FuncCall(ASTNode* node) {
    // Returns code 4 on failure

    // If function is not in symtable, return [error_code]
    // If amount of arguments is different, return [error_code]
    // For each parameter,
    //   If types are different, return [error_code]

    return 0;
}

int SemCheck_RelationalExpression(ASTNode* node) {
    // Run SemCheck_Expression
    // If not relational expression, return [error_code]
    // TODO: If a const expression, should we care?
    return 0;
}

int SemCheck_Block(ASTNode* node) {
    // Mutable variables must be mutated
    // Immutable variables must be immutable
    // Every variable defined must be used before it goes out of scope

    // Run SemCheck_Statement for each statement in the block body
    return 0;
}

int SemCheck_ConditionalBlock(ASTNode* node) {
    // Get condition from left
    // Run SemCheck_RelationalExpression on condition
    // If condition is always true or always [error_code], return [error_code]

    // Get block from right
    // Run SemCheck_Block on block

    return 0;
}
