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
    match(lexer, TOKEN_KEYWORD_CONST); // 'const'
    match(lexer, TOKEN_ID); // 'ifj'
    match(lexer, TOKEN_ASSIGNMENT); // '='
    match(lexer, TOKEN_AT); // '@'
    match(lexer, TOKEN_ID); // 'import'
    match(lexer, TOKEN_LEFT_ROUND_BRACKET);
    match(lexer, TOKEN_STRING_LITERAL);
    match(lexer, TOKEN_RIGHT_ROUND_BRACKET);
    match(lexer, TOKEN_SEMICOLON);
}

void parseFunctionDefList(lexer_t *lexer) {
    while ((token_type_t)token.type == TOKEN_KEYWORD_PUB) {
        parseFunctionDef(lexer);
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

    parseStatementList(lexer);            // Parse the function body statements

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
    while ((token_type_t)token.type == TOKEN_COMMA) {
        match(lexer, TOKEN_COMMA);       // Comma for separating parameters
        match(lexer, TOKEN_ID);          // Next parameter name
        match(lexer, TOKEN_COLON);       // Colon
        parseType(lexer);                // Type
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
        fprintf(stderr,"Syntax error: invalid type\n");
        exit(2);
    }
}

void parseStatementList(lexer_t *lexer) {
    while ((token_type_t)token.type != TOKEN_RIGHT_CURLY_BRACKET) {  // Stop when '}' is encountered
        parseStatement(lexer);                        // Parse each statement in the list
    }
}


void parseStatement(lexer_t *lexer) {
    switch ((token_type_t)token.type) {
        case TOKEN_KEYWORD_CONST:
            parseConstDeclaration(lexer);
            break;
        case TOKEN_KEYWORD_VAR:
            parseVarDeclaration(lexer);
            break;
        case TOKEN_ID:
            parseAssignmentOrFunctionCall(lexer);
            break;
        case TOKEN_KEYWORD_IF:
            parseIfStatement(lexer);
            break;
        case TOKEN_KEYWORD_WHILE:
            parseWhileStatement(lexer);
            break;
        case TOKEN_KEYWORD_RETURN:
            parseReturnStatement(lexer);
            break;
        default:
            // Handle syntax error for unexpected token
            fprintf(stderr,"Syntax error: unexpected token in statement\n");
            exit(2);
    }
}

void parseConstDeclaration(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_CONST);  // Match 'const' keyword
    match(lexer, TOKEN_ID);             // Match the identifier (constant name)
    match(lexer, TOKEN_COLON);          // Match the colon ':'
    parseType(lexer);                   // Parse the type
    match(lexer, TOKEN_ASSIGNMENT);     // Match the assignment operator '='
    parseExpression(lexer);             // Parse the expression (constant value)
    match(lexer, TOKEN_SEMICOLON);      // Match the semicolon ';'
}

void parseExpression(lexer_t *lexer) {
    parseSimpleExpression(lexer);  // Parse the simple expression (like a term or factor)

    // Check if the current token is a relational operator (like <, >, ==, etc.)
    if ((token_type_t)token.type == TOKEN_LESS_THAN
    || (token_type_t)token.type == TOKEN_LESS_THAN_OR_EQUAL_TO
    || (token_type_t)token.type == TOKEN_GREATER_THAN
    || (token_type_t)token.type == TOKEN_GREATER_THAN_OR_EQUAL_TO
    || (token_type_t)token.type == TOKEN_EQUAL_TO
    || (token_type_t)token.type == TOKEN_NOT_EQUAL_TO) {
        parseRelationalTail(lexer);  // Parse the relational tail if a relational operator is found
    }
}

void parseRelationalTail(lexer_t *lexer) {
    // Match the relational operator (already verified in parseExpression)
    switch ((token_type_t)token.type) {
        case TOKEN_LESS_THAN:
            match(lexer, TOKEN_LESS_THAN);
            break;
        case TOKEN_LESS_THAN_OR_EQUAL_TO:
            match(lexer, TOKEN_LESS_THAN_OR_EQUAL_TO);
            break;
        case TOKEN_GREATER_THAN:
            match(lexer, TOKEN_GREATER_THAN);
            break;
        case TOKEN_GREATER_THAN_OR_EQUAL_TO:
            match(lexer, TOKEN_GREATER_THAN_OR_EQUAL_TO);
            break;
        case TOKEN_EQUAL_TO:
            match(lexer, TOKEN_EQUAL_TO);
            break;
        case TOKEN_NOT_EQUAL_TO:
            match(lexer, TOKEN_NOT_EQUAL_TO);
            break;
        default:
            // Handle error if not a relational operator
            fprintf(stderr,"Syntax error: unexpected token in relational expression\n");
            exit(2);
    }
    parseSimpleExpression(lexer);  // Parse the expression after the relational operator
}

void parseSimpleExpression(lexer_t *lexer) {
    parseTerm(lexer);  // Parse the term (a variable, literal, or subexpression)

    // Handle addition and subtraction in expressions
    while ((token_type_t)token.type == TOKEN_ADDITION
        || (token_type_t)token.type == TOKEN_SUBTRACTION) {
        if ((token_type_t)token.type == TOKEN_ADDITION) {
            match(lexer, TOKEN_ADDITION);
        } else {
            match(lexer, TOKEN_SUBTRACTION);
        }
        parseTerm(lexer);  // Parse the next term after the operator
    }
}

void parseTerm(lexer_t *lexer) {
    parseFactor(lexer);  // Parse the factor (like a literal or subexpression)

    // Handle multiplication and division in expressions
    while ((token_type_t)token.type == TOKEN_MULTIPLICATION
        || (token_type_t)token.type == TOKEN_DIVISION) {
        if ((token_type_t)token.type == TOKEN_MULTIPLICATION) {
            match(lexer, TOKEN_MULTIPLICATION);
        } else {
            match(lexer, TOKEN_DIVISION);
        }
        parseFactor(lexer);  // Parse the next factor after the operator
    }
}

void parseFactor(lexer_t *lexer) {
    if ((token_type_t)token.type == TOKEN_LEFT_ROUND_BRACKET) {  // If it's a subexpression in parentheses
        match(lexer, TOKEN_LEFT_ROUND_BRACKET);
        parseExpression(lexer);                    // Parse the subexpression
        match(lexer, TOKEN_RIGHT_ROUND_BRACKET);   // Ensure closing bracket is matched
    } else if ((token_type_t)token.type == TOKEN_ID) {           // If it's an identifier (variable or function call)
        match(lexer, TOKEN_ID);
        if ((token_type_t)token.type == TOKEN_LEFT_ROUND_BRACKET) {
            parseFunctionCall(lexer);              // Optionally, handle function call if '(' follows
        }
    } else if ((token_type_t)token.type == TOKEN_I32_LITERAL
        || (token_type_t)token.type == TOKEN_F64_LITERAL
        || (token_type_t)token.type == TOKEN_STRING_LITERAL
        || (token_type_t)token.type == TOKEN_KEYWORD_NULL) {
        match(lexer, (token_type_t)token.type);                  // Match the literal or null keyword
    } else {
        // Handle syntax error if no valid factor is found
        fprintf(stderr,"Syntax error: unexpected token in factor\n");
        exit(2);
    }
}

void parseFunctionCall(lexer_t *lexer) {
    match(lexer, TOKEN_LEFT_ROUND_BRACKET);  // Match '('

    // If there are arguments in the function call
    if ((token_type_t)token.type != TOKEN_RIGHT_ROUND_BRACKET) {
        parseExpression(lexer);  // Parse the first argument expression

        // Parse any additional arguments, separated by commas
        while ((token_type_t)token.type == TOKEN_COMMA) {
            match(lexer, TOKEN_COMMA);       // Match ','
            parseExpression(lexer);          // Parse the next argument
        }
    }

    match(lexer, TOKEN_RIGHT_ROUND_BRACKET); // Match ')'
}

void parseVarDeclaration(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_VAR);  // Match 'var' keyword
    match(lexer, TOKEN_ID);           // Match the identifier (variable name)
    match(lexer, TOKEN_COLON);        // Match the colon ':'
    parseType(lexer);                 // Parse the type
    match(lexer, TOKEN_ASSIGNMENT);   // Match the assignment operator '='
    parseExpression(lexer);           // Parse the expression (variable value)
    match(lexer, TOKEN_SEMICOLON);    // Match the semicolon ';'
}

void parseAssignmentOrFunctionCall(lexer_t *lexer) {
    match(lexer, TOKEN_ID);  // Match the identifier

    if ((token_type_t)token.type == TOKEN_ASSIGNMENT) {
        match(lexer, TOKEN_ASSIGNMENT);  // Match '='
        parseExpression(lexer);          // Parse the expression for assignment
        match(lexer, TOKEN_SEMICOLON);   // Match the semicolon
    } else if ((token_type_t)token.type == TOKEN_LEFT_ROUND_BRACKET) {
        parseFunctionCall(lexer);        // Parse the function call
        match(lexer, TOKEN_SEMICOLON);   // Match the semicolon after the function call
    } else {
        // Handle error for unexpected token after identifier
        fprintf(stderr,"Syntax error: unexpected token after identifier\n");
        exit(2);
    }
}

void parseIfStatement(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_IF);          // Match 'if'
    match(lexer, TOKEN_LEFT_ROUND_BRACKET);  // Match '('
    parseExpression(lexer);                  // Parse the expression (could be a nullable type)
    match(lexer, TOKEN_RIGHT_ROUND_BRACKET); // Match ')'

    // Handle nullable binding
    if ((token_type_t)token.type == TOKEN_VERTICAL_BAR) {
        match(lexer, TOKEN_VERTICAL_BAR);    // Match '|'
        match(lexer, TOKEN_ID);              // Match id_bez_null (identifier)
        match(lexer, TOKEN_VERTICAL_BAR);    // Match closing '|'
    }

    match(lexer, TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    parseStatementList(lexer);               // Parse statements inside the 'if' block
    match(lexer, TOKEN_RIGHT_CURLY_BRACKET); // Match '}'

    // Optional 'else' block
    if ((token_type_t)token.type == TOKEN_KEYWORD_ELSE) {
        match(lexer, TOKEN_KEYWORD_ELSE);    // Match 'else'
        match(lexer, TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
        parseStatementList(lexer);               // Parse statements inside 'else'
        match(lexer, TOKEN_RIGHT_CURLY_BRACKET); // Match '}'
    }
}

void parseWhileStatement(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_WHILE);     // Match 'while'
    match(lexer, TOKEN_LEFT_ROUND_BRACKET); // Match '('
    parseExpression(lexer);                // Parse the condition expression
    match(lexer, TOKEN_RIGHT_ROUND_BRACKET); // Match ')'
    match(lexer, TOKEN_LEFT_CURLY_BRACKET);  // Match '{'
    parseStatementList(lexer);             // Parse the statements inside the loop
    match(lexer, TOKEN_RIGHT_CURLY_BRACKET); // Match '}'
}

void parseReturnStatement(lexer_t *lexer) {
    match(lexer, TOKEN_KEYWORD_RETURN);   // Match 'return'
    if ((token_type_t)token.type != TOKEN_SEMICOLON) {
        parseExpression(lexer);           // Parse the return expression if present
    }
    match(lexer, TOKEN_SEMICOLON);        // Match the semicolon ';'
}
