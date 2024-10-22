//
// Created by malbert on 10/5/24.
//
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "structs/stack.h"
#include "parser/parser.h"
#include <stdio.h>
#include <stdlib.h>

Token token;
TokenArray *tokenArr;

void parseInit(TokenArray* array){
    Stack *expStack = (Stack *)malloc(sizeof(Stack));
    initializeStack(expStack, 100);
    tokenArr = array;
    token = get_next_token();
    parseProgram();
}

void parseProgram() {
    //parseProlog();
    parseFunctionDefList();
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

void parseProlog() {
    match(TOKEN_KEYWORD_CONST); // 'const'
    match(TOKEN_ID); // 'ifj'
    match(TOKEN_ASSIGNMENT); // '='
    match(TOKEN_AT); // '@'
    match(TOKEN_ID); // 'import'
    match(TOKEN_LEFT_ROUND_BRACKET);
    match(TOKEN_STRING_LITERAL);
    match(TOKEN_RIGHT_ROUND_BRACKET);
    match(TOKEN_SEMICOLON);
}

void parseFunctionDefList() {
    while ((TokenType)token.type == TOKEN_KEYWORD_PUB) {
        parseFunctionDef();
    }
}

void parseFunctionDef() {
    match(TOKEN_KEYWORD_PUB);      // Matches 'pub'
    match(TOKEN_KEYWORD_FN);       // Matches 'fn'
    match(TOKEN_ID);               // Function identifier
    match(TOKEN_LEFT_ROUND_BRACKET); // Opening parenthesis

    parseParamList();                // Parse the function's parameter list

    match(TOKEN_RIGHT_ROUND_BRACKET); // Closing parenthesis

    parseReturnType();               // Parse the return type (void or other types)

    match(TOKEN_LEFT_CURLY_BRACKET);  // Opening curly bracket for function body

    parseStatementList();            // Parse the function body statements

    match(TOKEN_RIGHT_CURLY_BRACKET); // Closing curly bracket
}

void parseParamList() {
    if ((TokenType)token.type == TOKEN_ID) {
        match(TOKEN_ID);          // Parameter name
        match(TOKEN_COLON);       // Colon
        parseType();                // Type
        parseParamListTail();       // Check for additional parameters
    }
    // If no parameters, the function definition will skip this part
}

void parseParamListTail() {
    while ((TokenType)token.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);       // Comma for separating parameters
        match(TOKEN_ID);          // Next parameter name
        match(TOKEN_COLON);       // Colon
        parseType();                // Type
    }
}

void parseReturnType() {
    if ((TokenType)token.type == TOKEN_KEYWORD_VOID) {
        match(TOKEN_KEYWORD_VOID); // Matches 'void'
    } else {
        parseType();                 // Parse the type if not void
    }
}

void parseType() {
    if ((TokenType)token.type == TOKEN_KEYWORD_I32) {
        match(TOKEN_KEYWORD_I32);  // Matches 'i32'
    } else if ((TokenType)token.type == TOKEN_KEYWORD_F64) {
        match(TOKEN_KEYWORD_F64);  // Matches 'f64'
    } else if ((TokenType)token.type == TOKEN_LEFT_SQUARE_BRACKET) {
        match(TOKEN_LEFT_SQUARE_BRACKET);
        match(TOKEN_RIGHT_SQUARE_BRACKET);
        match(TOKEN_KEYWORD_U8);  // Matches '[]u8'
    } else if ((TokenType)token.type == TOKEN_KEYWORD_I32_NULLABLE) {
        match(TOKEN_KEYWORD_I32_NULLABLE);  // Matches '?i32'
    } else if ((TokenType)token.type == TOKEN_KEYWORD_F64_NULLABLE) {
        match(TOKEN_KEYWORD_F64_NULLABLE);  // Matches '?f64'
    } else if ((TokenType)token.type == TOKEN_KEYWORD_U8_ARRAY_NULLABLE) {
        match(TOKEN_KEYWORD_U8_ARRAY_NULLABLE);  // Matches '?[]u8'
    } else {
        // Handle error for invalid type
        fprintf(stderr,"Syntax error: invalid type\n");
        exit(2);
    }
}

void parseStatementList() {
    while ((TokenType)token.type != TOKEN_RIGHT_CURLY_BRACKET) {  // Stop when '}' is encountered
        parseStatement();                        // Parse each statement in the list
    }
}


void parseStatement() {
    switch ((TokenType)token.type) {
        case TOKEN_KEYWORD_CONST:
            parseConstDeclaration();
            break;
        case TOKEN_KEYWORD_VAR:
            parseVarDeclaration();
            break;
        case TOKEN_ID:
            parseAssignmentOrFunctionCall();
            break;
        case TOKEN_KEYWORD_IF:
            parseIfStatement();
            break;
        case TOKEN_KEYWORD_WHILE:
            parseWhileStatement();
            break;
        case TOKEN_KEYWORD_RETURN:
            parseReturnStatement();
            break;
        default:
            // Handle syntax error for unexpected token
            fprintf(stderr,"Syntax error: unexpected token in statement\n");
            exit(2);
    }
}

void parseConstDeclaration() {
    match(TOKEN_KEYWORD_CONST);  // Match 'const' keyword
    match(TOKEN_ID);             // Match the identifier (constant name)
    parseVarType();             // Parse the type
    match(TOKEN_ASSIGNMENT);     // Match the assignment operator '='
    parseExpression();             // Parse the expression (constant value)
    match(TOKEN_SEMICOLON);      // Match the semicolon ';'
}

void parseVarType(){
    if ((TokenType)token.type != TOKEN_ASSIGNMENT){
        match(TOKEN_COLON);
        parseType();
    }
}

void parseExpression() {
    parseSimpleExpression();  // Parse the simple expression (like a term or factor)

    // Check if the current token is a relational operator (like <, >, ==, etc.)
    if ((TokenType)token.type == TOKEN_LESS_THAN
    || (TokenType)token.type == TOKEN_LESS_THAN_OR_EQUAL_TO
    || (TokenType)token.type == TOKEN_GREATER_THAN
    || (TokenType)token.type == TOKEN_GREATER_THAN_OR_EQUAL_TO
    || (TokenType)token.type == TOKEN_EQUAL_TO
    || (TokenType)token.type == TOKEN_NOT_EQUAL_TO) {
        parseRelationalTail();  // Parse the relational tail if a relational operator is found
    }
}

void parseRelationalTail() {
    // Match the relational operator (already verified in parseExpression)
    switch ((TokenType)token.type) {
        case TOKEN_LESS_THAN:
            match(TOKEN_LESS_THAN);
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            match(TOKEN_LESS_THAN_OR_EQUAL_TO);
            break;
        case TOKEN_GREATER_THAN:
            match(TOKEN_GREATER_THAN);
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            match(TOKEN_GREATER_THAN_OR_EQUAL_TO);
            break;
        case TOKEN_EQUAL_TO:
            match(TOKEN_EQUAL_TO);
            break;
        case TOKEN_NOT_EQUAL_TO:
            match(TOKEN_NOT_EQUAL_TO);
            break;
        default:
            // Handle error if not a relational operator
            fprintf(stderr,"Syntax error: unexpected token in relational expression\n");
            exit(2);
    }
    parseSimpleExpression();  // Parse the expression after the relational operator
}

void parseSimpleExpression() {
    parseTerm();  // Parse the term (a variable, literal, or subexpression)

    // Handle addition and subtraction in expressions
    while ((TokenType)token.type == TOKEN_ADDITION
        || (TokenType)token.type == TOKEN_SUBTRACTION) {
        if ((TokenType)token.type == TOKEN_ADDITION) {
            match(TOKEN_ADDITION);
        } else {
            match(TOKEN_SUBTRACTION);
        }
        parseTerm();  // Parse the next term after the operator
    }
}

void parseTerm() {
    parseFactor();  // Parse the factor (like a literal or subexpression)

    // Handle multiplication and division in expressions
    while ((TokenType)token.type == TOKEN_MULTIPLICATION
        || (TokenType)token.type == TOKEN_DIVISION) {
        if ((TokenType)token.type == TOKEN_MULTIPLICATION) {
            match(TOKEN_MULTIPLICATION);
        } else {
            match(TOKEN_DIVISION);
        }
        parseFactor();  // Parse the next factor after the operator
    }
}

void parseFactor() {
    if ((TokenType)token.type == TOKEN_LEFT_ROUND_BRACKET) {  // If it's a subexpression in parentheses
        match(TOKEN_LEFT_ROUND_BRACKET);
        parseExpression();                    // Parse the subexpression
        match(TOKEN_RIGHT_ROUND_BRACKET);   // Ensure closing bracket is matched
    } else if ((TokenType)token.type == TOKEN_ID) {           // If it's an identifier (variable or function call)
        match(TOKEN_ID);
        if ((TokenType)token.type == TOKEN_LEFT_ROUND_BRACKET) {
            parseFunctionCall();              // Optionally, handle function call if '(' follows
        }
    } else if ((TokenType)token.type == TOKEN_I32_LITERAL
        || (TokenType)token.type == TOKEN_F64_LITERAL
        || (TokenType)token.type == TOKEN_STRING_LITERAL
        || (TokenType)token.type == TOKEN_KEYWORD_NULL) {
        match((TokenType)token.type);                  // Match the literal or null keyword
    } else {
        // Handle syntax error if no valid factor is found
        fprintf(stderr,"Syntax error: unexpected token in factor\n");
        exit(2);
    }
}

void parseFunctionCall() {
    match(TOKEN_LEFT_ROUND_BRACKET);  // Match '('

    // If there are arguments in the function call
    if ((TokenType)token.type != TOKEN_RIGHT_ROUND_BRACKET) {
        parseExpression();  // Parse the first argument expression

        // Parse any additional arguments, separated by commas
        while ((TokenType)token.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);       // Match ','
            parseExpression();          // Parse the next argument
        }
    }

    match(TOKEN_RIGHT_ROUND_BRACKET); // Match ')'
}

void parseVarDeclaration() {
    match(TOKEN_KEYWORD_VAR);  // Match 'var' keyword
    match(TOKEN_ID);           // Match the identifier (variable name)
    parseVarType();                 // Parse the type
    match(TOKEN_ASSIGNMENT);   // Match the assignment operator '='
    parseExpression();           // Parse the expression (variable value)
    match(TOKEN_SEMICOLON);    // Match the semicolon ';'
}

void parseAssignmentOrFunctionCall() {
    match(TOKEN_ID);  // Match the identifier

    if ((TokenType)token.type == TOKEN_ASSIGNMENT) {
        match(TOKEN_ASSIGNMENT);  // Match '='
        parseExpression();          // Parse the expression for assignment
        match(TOKEN_SEMICOLON);   // Match the semicolon
    } else if ((TokenType)token.type == TOKEN_LEFT_ROUND_BRACKET) {
        parseFunctionCall();        // Parse the function call
        match(TOKEN_SEMICOLON);   // Match the semicolon after the function call
    } else {
        // Handle error for unexpected token after identifier
        fprintf(stderr,"Syntax error: unexpected token after identifier\n");
        exit(2);
    }
}

void parseIfStatement() {
    match(TOKEN_KEYWORD_IF);          // Match 'if'
    match(TOKEN_LEFT_ROUND_BRACKET);  // Match '('
    parseExpression();                  // Parse the expression (could be a nullable type)
    match(TOKEN_RIGHT_ROUND_BRACKET); // Match ')'

    // Handle nullable binding
    if ((TokenType)token.type == TOKEN_VERTICAL_BAR) {
        match(TOKEN_VERTICAL_BAR);    // Match '|'
        match(TOKEN_ID);              // Match id_bez_null (identifier)
        match(TOKEN_VERTICAL_BAR);    // Match closing '|'
    }

    match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    parseStatementList();               // Parse statements inside the 'if' block
    match(TOKEN_RIGHT_CURLY_BRACKET); // Match '}'

    // Optional 'else' block
    if ((TokenType)token.type == TOKEN_KEYWORD_ELSE) {
        match(TOKEN_KEYWORD_ELSE);    // Match 'else'
        match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
        parseStatementList();               // Parse statements inside 'else'
        match(TOKEN_RIGHT_CURLY_BRACKET); // Match '}'
    }
}

void parseWhileStatement() {
    match(TOKEN_KEYWORD_WHILE);     // Match 'while'
    match(TOKEN_LEFT_ROUND_BRACKET); // Match '('
    parseExpression();                // Parse the condition expression
    match(TOKEN_RIGHT_ROUND_BRACKET); // Match ')'
    match(TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    parseStatementList();             // Parse the statements inside the loop
    match(TOKEN_RIGHT_CURLY_BRACKET); // Match '}'
}

void parseReturnStatement() {
    match(TOKEN_KEYWORD_RETURN);   // Match 'return'
    if ((TokenType)token.type != TOKEN_SEMICOLON) {
        parseExpression();           // Parse the return expression if present
    }
    match(TOKEN_SEMICOLON);        // Match the semicolon ';'
}

Token get_next_token(){
    static unsigned int stat_index = 0;
    return tokenArr->tokens[stat_index++];
}