//
// Created by malbert on 10/5/24.
//
#include "lexer/token.h"
#include "structs/ast.h"
#include "parser/parser.h"
#include <stdio.h>
#include <stdlib.h>

Token token;
TokenArray *tokenArr;
unsigned int stat_index = 0;

ASTNode* parseInit(TokenArray* array) {
    tokenArr = array;
    token = get_next_token();  // Initialize the first token
    return parseProgram();  // Parse the program and store the AST root
}

ASTNode* parseProgram() {
    // Parse the prolog first
    ASTNode* prologNode = parseProlog();

    // Parse the list of function definitions
    ASTNode* functionListNode = parseFunctionDefList();

    // Create the root node for the program
    ASTNode* root = createASTNode("Program", NULL);

    // Attach the prolog and function list as children of the program root
    root->left = prologNode;
    prologNode->next = functionListNode;

    return root;  // Return the root of the AST
}

void match(TokenType expected) {
    if ((TokenType)token.type == expected) {
        token = get_next_token(); // Move to the next token
    } else {
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", expected, (TokenType)token.type);
        exit(2); // or handle error gracefully
    }
}

ASTNode* parseProlog() {
    ASTNode* prologNode = createASTNode("Prolog", NULL);

    match(TOKEN_KEYWORD_CONST);  // Match 'const'
    match(TOKEN_ID);             // Match 'ifj'
    match(TOKEN_ASSIGNMENT);     // Match '='
    match(TOKEN_AT);             // Match '@'
    match(TOKEN_ID);             // Match 'import'
    match(TOKEN_LEFT_ROUND_BRACKET);
    match(TOKEN_STRING_LITERAL);
    match(TOKEN_RIGHT_ROUND_BRACKET);
    match(TOKEN_SEMICOLON);

    return prologNode;  // Return the prolog node
}


ASTNode* parseFunctionDefList() {
    ASTNode* head = NULL;  // Head of the function definition list
    ASTNode* current = NULL;  // Current node in the list

    while (token.type == TOKEN_KEYWORD_PUB) {  // While there are functions to parse
        ASTNode* funcDefNode = parseFunctionDef();  // Parse each function definition

        if (head == NULL) {
            head = funcDefNode;  // First function becomes the head
        } else if (current != NULL){
            current->next = funcDefNode;  // Link the new function to the previous one
        }

        current = funcDefNode;  // Move the current pointer to the newly added function
    }

    return head;  // Return the head of the function definition list
}


ASTNode* parseFunctionDef() {
    match(TOKEN_KEYWORD_PUB);      // Matches 'pub'
    match(TOKEN_KEYWORD_FN);       // Matches 'fn'

    // Capture the function name
    char* functionName = strdup(token.attribute.str);
    match(TOKEN_ID);

    // Create an AST node for the function definition
    ASTNode* funcNode = createASTNode("FunctionDef", functionName);

    // Parse parameters and attach to the function node
    match(TOKEN_LEFT_ROUND_BRACKET);
    funcNode->left = parseParamList();  // Attach parameter list as the left child
    match(TOKEN_RIGHT_ROUND_BRACKET);

    // Parse return type
    funcNode->right = parseReturnType();  // Attach return type as the right child

    // Parse function body and add it as the next node
    match(TOKEN_LEFT_CURLY_BRACKET);
    funcNode->next = parseStatementList();  // Attach the function body statements
    match(TOKEN_RIGHT_CURLY_BRACKET);

    return funcNode;  // Return the completed function definition node
}


ASTNode* parseParamList() {
    ASTNode* head = NULL;

    if (token.type == TOKEN_ID) {
        // Parse the first parameter
        char* paramName = strdup(token.attribute.str);
        match(TOKEN_ID);
        match(TOKEN_COLON);
        ASTNode* paramType = parseType();

        head = createASTNode("Parameter", paramName);
        head->left = paramType;

        // Parse additional parameters
        head->next = parseParamListTail();
    }

    return head;
}

ASTNode* parseParamListTail() {
    if (token.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);

        // Parse the next parameter
        char* paramName = strdup(token.attribute.str);
        match(TOKEN_ID);
        match(TOKEN_COLON);
        ASTNode* paramType = parseType();

        ASTNode* paramNode = createASTNode("Parameter", paramName);
        paramNode->left = paramType;

        // Continue parsing additional parameters
        paramNode->next = parseParamListTail();
        return paramNode;
    }

    return NULL;  // No more parameters
}


ASTNode* parseReturnType() {
    ASTNode* returnTypeNode = NULL;

    if (token.type == TOKEN_KEYWORD_VOID) {
        returnTypeNode = createASTNode("ReturnType", "void");
        match(TOKEN_KEYWORD_VOID);
    } else {
        returnTypeNode = parseType();  // Use parseType() for non-void types
    }

    return returnTypeNode;
}


ASTNode* parseType() {
    ASTNode* typeNode = NULL;

    if (token.type == TOKEN_KEYWORD_I32) {
        typeNode = createASTNode("Type", "i32");
        match(TOKEN_KEYWORD_I32);
    } else if (token.type == TOKEN_KEYWORD_F64) {
        typeNode = createASTNode("Type", "f64");
        match(TOKEN_KEYWORD_F64);
    } else if (token.type == TOKEN_KEYWORD_U8_ARRAY) {
        typeNode = createASTNode("Type", "[]u8");
        match(TOKEN_KEYWORD_U8_ARRAY);
    } else if (token.type == TOKEN_KEYWORD_I32_NULLABLE) {
        typeNode = createASTNode("Type", "?i32");
        match(TOKEN_KEYWORD_I32_NULLABLE);
    } else if (token.type == TOKEN_KEYWORD_F64_NULLABLE) {
        typeNode = createASTNode("Type", "?f64");
        match(TOKEN_KEYWORD_F64_NULLABLE);
    } else if (token.type == TOKEN_KEYWORD_U8_ARRAY_NULLABLE) {
        typeNode = createASTNode("Type", "?[]u8");
        match(TOKEN_KEYWORD_U8_ARRAY_NULLABLE);
    } else {
        // Handle error for invalid type
        fprintf(stderr, "Syntax error: invalid type\n");
        exit(2);
    }

    return typeNode;  // Return the type node for the AST
}

ASTNode* parseStatementList() {
    ASTNode* head = NULL;      // Head of the statement list
    ASTNode* current = NULL;   // Current node in the list

    while (token.type != TOKEN_RIGHT_CURLY_BRACKET) {  // Stop when '}' is encountered
        ASTNode* stmtNode = parseStatement();  // Parse each statement and get its AST node

        if (head == NULL) {
            head = stmtNode;  // The first statement becomes the head
        } else if (current != NULL){
            current->next = stmtNode;  // Link the new statement to the previous one
        }

        current = stmtNode;  // Move the current pointer to the newly added statement
    }

    return head;  // Return the head of the statement list
}



ASTNode* parseStatement() {
    ASTNode* stmtNode = NULL;

    switch (token.type) {
        case TOKEN_KEYWORD_CONST:
            stmtNode = parseConstDeclaration();  // Parse const declaration
            break;
        case TOKEN_KEYWORD_VAR:
            stmtNode = parseVarDeclaration();  // Parse variable declaration
            break;
        case TOKEN_ID:
            stmtNode = parseAssignmentOrFunctionCall();  // Parse assignment or function call
            break;
        case TOKEN_KEYWORD_IF:
            stmtNode = parseIfStatement();  // Parse if statement
            break;
        case TOKEN_KEYWORD_WHILE:
            stmtNode = parseWhileStatement();  // Parse while statement
            break;
        case TOKEN_KEYWORD_RETURN:
            stmtNode = parseReturnStatement();  // Parse return statement
            break;
        default:
            // Handle syntax error for unexpected token
            fprintf(stderr, "Syntax error: unexpected token in statement\n");
            exit(2);
    }

    return stmtNode;
}

ASTNode* parseConstDeclaration() {
    match(TOKEN_KEYWORD_CONST);  // Match 'const' keyword

    // Capture the constant name
    char* constName = strdup(token.attribute.str);
    match(TOKEN_ID);  // Match the identifier (constant name)

    // Use parseVarType to handle optional type annotation
    ASTNode* typeNode = parseVarType();  // Returns the type node or NULL if no type

    match(TOKEN_ASSIGNMENT);  // Match '='
    ASTNode* exprNode = parseExpression();  // Parse the constant's assigned value

    // Create the AST node for the const declaration
    ASTNode* constNode = createASTNode("ConstDeclaration", constName);
    constNode->left = typeNode;    // Attach type as left child (if available)
    constNode->right = exprNode;   // Attach the expression as right child

    match(TOKEN_SEMICOLON);  // Match ';'

    return constNode;
}



ASTNode* parseVarType() {
    ASTNode* typeNode = NULL;

    if (token.type == TOKEN_COLON) {  // Check if there's a type annotation
        match(TOKEN_COLON);
        typeNode = parseType();  // Parse and create the type node
    }

    return typeNode;  // Return the type node or NULL if no type specified
}


ASTNode* parseExpression() {
    ASTNode* left = parseSimpleExpression();  // Parse the initial simple expression

    // Use parseRelationalTail to handle additional relational operators
    ASTNode* result = parseRelationalTail(left);
    return result;  // Start the tail parsing with the initial left expression
}

ASTNode* parseRelationalTail(ASTNode* left) {
    // Check if the current token is a relational operator
    if (token.type == TOKEN_LESS_THAN || token.type == TOKEN_LESS_THAN_OR_EQUAL_TO ||
        token.type == TOKEN_GREATER_THAN || token.type == TOKEN_GREATER_THAN_OR_EQUAL_TO ||
        token.type == TOKEN_EQUAL_TO || token.type == TOKEN_NOT_EQUAL_TO) {

        // Capture the operator and move to the next token
        char* operator = strdup(token.attribute.str);
        match(token.type);

        // Parse the right operand
        ASTNode* right = parseSimpleExpression();

        // Create an AST node for the relational operation
        ASTNode* opNode = createBinaryASTNode(operator, left, right);

        // Recursively call parseRelationalTail with the new opNode as the left operand
        return parseRelationalTail(opNode);
    }

    return left;  // Return the completed expression node
}


ASTNode* parseSimpleExpression() {
    ASTNode* left = parseTerm();  // Parse the first term (left operand)

    // Handle addition and subtraction operators
    while (token.type == TOKEN_ADDITION || token.type == TOKEN_SUBTRACTION) {
        char* operator = strdup(token.attribute.str);  // Capture the operator
        match(token.type);  // Consume the operator

        // Parse the next term (right operand)
        ASTNode* right = parseTerm();

        // Create a binary operation node
        left = createBinaryASTNode(operator, left, right);  // Update left with new binary node
    }

    return left;  // Return the completed simple expression node
}


ASTNode* parseTerm() {
    ASTNode* left = parseFactor();  // Parse the initial factor (left operand)

    // Handle multiplication and division
    while (token.type == TOKEN_MULTIPLICATION || token.type == TOKEN_DIVISION) {
        char* operator = strdup(token.attribute.str);  // Capture the operator
        match(token.type);  // Consume the operator

        // Parse the next factor (right operand)
        ASTNode* right = parseFactor();

        // Create a binary operation node
        left = createBinaryASTNode(operator, left, right);  // Update left with new binary node
    }

    return left;  // Return the completed term node
}


ASTNode* parseFactor() {
    ASTNode* factorNode = NULL;

    if (token.type == TOKEN_LEFT_ROUND_BRACKET) {  // For expressions in parentheses
        match(TOKEN_LEFT_ROUND_BRACKET);
        factorNode = parseExpression();  // Parse the sub-expression inside parentheses
        match(TOKEN_RIGHT_ROUND_BRACKET);
    } else if (token.type == TOKEN_ID) {  // For identifiers (variables or function calls)
        char* identifier = strdup(token.attribute.str);
        match(TOKEN_ID);

        if (token.type == TOKEN_LEFT_ROUND_BRACKET) {  // If it’s a function call
            factorNode = createASTNode("FunctionCall", identifier);
            factorNode->left = parseFunctionCall();  // Attach arguments
        } else {
            factorNode = createASTNode("Identifier", identifier);  // Variable reference
        }
    } else if (token.type == TOKEN_I32_LITERAL || token.type == TOKEN_F64_LITERAL ||
               token.type == TOKEN_STRING_LITERAL || token.type == TOKEN_KEYWORD_NULL) {
        // For literals and `null`
        char* literalValue = strdup(token.attribute.str);
        factorNode = createASTNode("Literal", literalValue);  // Literal node
        match(token.type);  // Consume the literal or `null`
    } else {
        // Handle syntax error if no valid factor is found
        fprintf(stderr, "Syntax error: unexpected token in factor\n");
        exit(2);
    }

    return factorNode;
}


ASTNode* parseFunctionCall() {
    match(TOKEN_LEFT_ROUND_BRACKET);  // Match '('

    // Create the root node for the function call arguments
    ASTNode* argsHead = NULL;
    ASTNode* currentArg = NULL;

    // Parse arguments if any
    if (token.type != TOKEN_RIGHT_ROUND_BRACKET) {
        argsHead = parseExpression();  // Parse the first argument expression
        currentArg = argsHead;

        // Parse additional arguments, if any, separated by commas
        while (token.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);  // Match ','

            // Parse the next argument expression
            ASTNode* nextArg = parseExpression();
            currentArg->next = nextArg;  // Link arguments
            currentArg = nextArg;
        }
    }

    match(TOKEN_RIGHT_ROUND_BRACKET);  // Match ')'

    return argsHead;  // Return the head of the argument list
}


ASTNode* parseVarDeclaration() {
    match(TOKEN_KEYWORD_VAR);  // Match 'var' keyword

    // Capture variable name
    char* varName = strdup(token.attribute.str);
    match(TOKEN_ID);  // Match the identifier (variable name)

    // Use parseVarType to handle optional type annotation
    ASTNode* typeNode = parseVarType();  // Returns the type node or NULL if no type

    match(TOKEN_ASSIGNMENT);  // Match '='
    ASTNode* exprNode = parseExpression();  // Parse the assigned expression

    // Create AST node for variable declaration
    ASTNode* varNode = createASTNode("VarDeclaration", varName);
    varNode->left = typeNode;   // Attach type as left child (if available)
    varNode->right = exprNode;  // Attach expression as right child

    match(TOKEN_SEMICOLON);  // Match ';'

    return varNode;
}

ASTNode* parseAssignmentOrFunctionCall() {
    // Capture the identifier (variable or function name)
    char* identifier = strdup(token.attribute.str);
    match(TOKEN_ID);  // Match the identifier

    if (token.type == TOKEN_ASSIGNMENT) {
        // Handle assignment
        match(TOKEN_ASSIGNMENT);  // Match '='

        ASTNode* exprNode = parseExpression();  // Parse the expression to assign
        ASTNode* assignNode = createASTNode("Assignment", identifier);  // Create an assignment node
        assignNode->left = exprNode;  // Attach the expression as the left child

        match(TOKEN_SEMICOLON);  // Match ';'
        return assignNode;
    } else if (token.type == TOKEN_LEFT_ROUND_BRACKET) {
        // Handle function call
        ASTNode* funcCallNode = createASTNode("FunctionCall", identifier);  // Create function call node
        funcCallNode->left = parseFunctionCall();  // Attach the argument list as the left child

        match(TOKEN_SEMICOLON);  // Match ';'
        return funcCallNode;
    } else {
        // Handle syntax error for unexpected token after identifier
        fprintf(stderr, "Syntax error: unexpected token after identifier\n");
        exit(2);
    }
}


ASTNode* parseIfStatement() {
    match(TOKEN_KEYWORD_IF);           // Match 'if'
    match(TOKEN_LEFT_ROUND_BRACKET);   // Match '('

    // Parse the condition expression
    ASTNode* conditionNode = parseExpression();
    match(TOKEN_RIGHT_ROUND_BRACKET);  // Match ')'

    // Handle nullable binding if present
    ASTNode* bindingNode = NULL;
    if (token.type == TOKEN_VERTICAL_BAR) {
        match(TOKEN_VERTICAL_BAR);      // Match '|'
        char* bindingVar = strdup(token.attribute.str);
        match(TOKEN_ID);                // Match identifier for nullable binding
        bindingNode = createASTNode("NullableBinding", bindingVar);
        match(TOKEN_VERTICAL_BAR);      // Match closing '|'
    }

    match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    ASTNode* trueBranch = parseStatementList();  // Parse statements in the true branch
    match(TOKEN_RIGHT_CURLY_BRACKET); // Match '}'

    // Optional 'else' block
    ASTNode* falseBranch = NULL;
    if (token.type == TOKEN_KEYWORD_ELSE) {
        match(TOKEN_KEYWORD_ELSE);      // Match 'else'
        match(TOKEN_LEFT_CURLY_BRACKET); // Match '{'
        falseBranch = parseStatementList();  // Parse statements in the false branch
        match(TOKEN_RIGHT_CURLY_BRACKET);    // Match '}'
    }

    // Create the AST node for the if statement
    ASTNode* ifNode = createASTNode("IfStatement", NULL);
    ifNode->left = conditionNode;   // Attach condition as the left child
    ifNode->right = trueBranch;     // Attach true branch as the right child
    ifNode->next = falseBranch;     // Attach false branch as the next node
    ifNode->binding = bindingNode;    // Attach nullable binding as an extra node if present

    return ifNode;
}


ASTNode* parseWhileStatement() {
    match(TOKEN_KEYWORD_WHILE);         // Match 'while'
    match(TOKEN_LEFT_ROUND_BRACKET);    // Match '('

    // Parse the condition expression
    ASTNode* conditionNode = parseExpression();
    match(TOKEN_RIGHT_ROUND_BRACKET);   // Match ')'

    // Handle nullable binding if present
    ASTNode* bindingNode = NULL;
    if (token.type == TOKEN_VERTICAL_BAR) {
        match(TOKEN_VERTICAL_BAR);      // Match '|'
        char* bindingVar = strdup(token.attribute.str);
        match(TOKEN_ID);                // Match identifier for nullable binding
        bindingNode = createASTNode("NullableBinding", bindingVar);
        match(TOKEN_VERTICAL_BAR);      // Match closing '|'
    }

    match(TOKEN_LEFT_CURLY_BRACKET);    // Match '{'
    ASTNode* bodyNode = parseStatementList();  // Parse the loop body
    match(TOKEN_RIGHT_CURLY_BRACKET);   // Match '}'

    // Create the AST node for the while statement
    ASTNode* whileNode = createASTNode("WhileStatement", NULL);
    whileNode->left = conditionNode;    // Attach condition as the left child
    whileNode->right = bodyNode;        // Attach the body as the right child
    whileNode->binding = bindingNode;   // Attach the nullable binding, if any

    return whileNode;
}


ASTNode* parseReturnStatement() {
    match(TOKEN_KEYWORD_RETURN);  // Match 'return'

    // Check if there is an expression to return
    ASTNode* exprNode = NULL;
    if (token.type != TOKEN_SEMICOLON) {
        exprNode = parseExpression();  // Parse the expression
    }

    // Create the AST node for the return statement
    ASTNode* returnNode = createASTNode("ReturnStatement", NULL);
    returnNode->left = exprNode;  // Attach the expression as the left child (if present)

    match(TOKEN_SEMICOLON);  // Match ';'
    return returnNode;
}


Token get_next_token(){
    if (stat_index < tokenArr->size - 1){
        return tokenArr->tokens[stat_index++];
    }
    exit(1);
}

Token get_previous_token(){
    if (stat_index != 0){
        return tokenArr->tokens[--stat_index];
    }
    exit(1);
}