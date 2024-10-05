//
// Created by malbert on 10/5/24.
//
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "structs/stack.h"
#include "parser/parser.h"

Token token;

void parser(lexer_t *lexer){
    Stack *expStack = (Stack *)malloc(sizeof(Stack));
    initializeStack(expStack, 100);
    parseProgram(lexer);
}

void parseProgram(lexer_t *lexer) {
    parseProlog(lexer);
    parseFunctionDefList(lexer);
}

void match(lexer_t *lexer, token_type_t expected) {
    if ((token_type_t)token.type == expected) {
        token = get_next_token(lexer); // Move to the next token
    } else {
        // Handle syntax error
        fprintf(stderr ,"Syntax error: expected %d, but got %d\n", expected, (token_type_t)token.type);
        exit(2); // or handle error gracefully
    }
}

void parseProlog(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_CONST);
    match(lexer, TOKEN_ID); // 'ifj'
    match(lexer, TOKEN_ASSIGNMENT);
    match(lexer, TOKEN_AT);
    match(lexer, TOKEN_ID); // 'import'
    match(lexer, TOKEN_LEFT_ROUND_BRACKET);
    match(lexer, TOKEN_STRING_LITERAL);
    match(lexer, TOKEN_RIGHT_ROUND_BRACKET);
    match(lexer, TOKEN_SEMICOLON);
}

void parseFunctionDefList(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_KEYWORD_PUB) {
        parseFunctionDef(lexer);
        parseFunctionDefList(lexer); // Recursion to handle multiple function definitions
    }
}

void parseFunctionDef(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_PUB);      // Matches 'pub'
    match(lexer, TOKEN_KEYWORD_FN);       // Matches 'fn'
    match(lexer, TOKEN_ID);               // Function identifier
    match(lexer, TOKEN_LEFT_ROUND_BRACKET); // Opening parenthesis

    parseParamList(lexer);                // Parse the function's parameter list

    match(lexer, TOKEN_RIGHT_ROUND_BRACKET); // Closing parenthesis

    parseReturnType(lexer);               // Parse the return type (void or other types)

    match(lexer, TOKEN_LEFT_CURLY_BRACKET);  // Opening curly bracket for function body

//    parseStatementList(lexer);            // Parse the function body statements

    match(lexer, TOKEN_RIGHT_CURLY_BRACKET); // Closing curly bracket
}

void parseParamList(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_ID) {
        match(lexer, TOKEN_ID);          // Parameter name
        match(lexer, TOKEN_COLON);       // Colon
        parseType(lexer);                // Type
        parseParamListTail(lexer);       // Check for additional parameters
    }
    // If no parameters, the function definition will skip this part
}

void parseParamListTail(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_COMMA) {
        match(lexer, TOKEN_COMMA);       // Comma for separating parameters
        match(lexer, TOKEN_ID);          // Next parameter name
        match(lexer, TOKEN_COLON);       // Colon
        parseType(lexer);                // Type
        parseParamListTail(lexer);       // Recursively handle the next parameters
    }
}

void parseReturnType(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_KEYWORD_VOID) {
        match(lexer, TOKEN_KEYWORD_VOID); // Matches 'void'
    } else {
        parseType(lexer);                 // Parse the type if not void
    }
}

void parseType(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_KEYWORD_I32) {
        match(lexer, TOKEN_KEYWORD_I32);  // Matches 'i32'
    } else if ((token_type_t)(token_type_t)token.type == TOKEN_KEYWORD_F64) {
        match(lexer, TOKEN_KEYWORD_F64);  // Matches 'f64'
    } else if ((token_type_t)token.type == TOKEN_KEYWORD_U8_ARRAY) {
        match(lexer, TOKEN_KEYWORD_U8_ARRAY);  // Matches '[]u8'
    } else if ((token_type_t)token.type == TOKEN_KEYWORD_I32_NULLABLE) {
        match(lexer, TOKEN_KEYWORD_I32_NULLABLE);  // Matches '?i32'
    } else if ((token_type_t)token.type == TOKEN_KEYWORD_F64_NULLABLE) {
        match(lexer, TOKEN_KEYWORD_F64_NULLABLE);  // Matches '?f64'
    } else if ((token_type_t)token.type == TOKEN_KEYWORD_U8_ARRAY_NULLABLE) {
        match(lexer, TOKEN_KEYWORD_U8_ARRAY_NULLABLE);  // Matches '?[]u8'
    } else {
        // Handle error for invalid type
        printf("Syntax error: invalid type\n");
        exit(2);
    }
}

void parseStatementList(lexer_t *lexer) {
    while (token.type != TOKEN_RIGHT_CURLY_BRACKET) {  // Stop when '}' is encountered
        parseStatement(lexer);                        // Parse each statement in the list
    }
}

//TODO: implement fucntions for non-terminal

//void parseStatement(lexer_t *lexer) {
//    switch (token.type) {
//        case TOKEN_KEYWORD_CONST:
//            parseConstDeclaration(lexer);
//            break;
//        case TOKEN_KEYWORD_VAR:
//            parseVarDeclaration(lexer);
//            break;
//        case TOKEN_ID:
//            parseAssignmentOrFunctionCall(lexer);
//            break;
//        case TOKEN_KEYWORD_IF:
//            parseIfStatement(lexer);
//            break;
//        case TOKEN_KEYWORD_WHILE:
//            parseWhileStatement(lexer);
//            break;
//        case TOKEN_KEYWORD_RETURN:
//            parseReturnStatement(lexer);
//            break;
//        default:
//            // Handle syntax error for unexpected token
//            printf("Syntax error: unexpected token in statement\n");
//            exit(2);
//    }
//}






