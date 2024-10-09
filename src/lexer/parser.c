// Created by Savin Ivan
// I think my implementation is too different from Nur's
// So I won't do anything with it

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//#include <regex.h>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "logging.h"


typedef enum {
    STATE_NORMAL,
    STATE_STRING,
    STATE_NEXT_LINE_STRING,
    STATE_COMMENT,
} LexerState;  // FSM which decides, how we aproach characters


// Token reader Buffer size
#define BUFFER_SIZE 256

// Array of keywords
const char* keywords[] = {
    "const", "else", "fn", "if", "i32", "f64", "null", "pub", 
    "return", "u8", "var", "void", "while"
};

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))   // Amount of key words

// checking is it a key word
bool isKeyword(const char* str) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true;  // Found
        }
    }
    return false;  // Not found
}

token_t processKeyword(const char* str) {
    token_t token;

    if (strcmp(str, "const") == 0) {
        token.type = TOKEN_KEYWORD_CONST;
    } else if (strcmp(str, "var") == 0) {
        token.type = TOKEN_KEYWORD_VAR;
    } else if (strcmp(str, "if") == 0) {
        token.type = TOKEN_KEYWORD_IF;
    } else if (strcmp(str, "else") == 0) {
        token.type = TOKEN_KEYWORD_ELSE;
    } else if (strcmp(str, "while") == 0) {
        token.type = TOKEN_KEYWORD_WHILE;
    } else if (strcmp(str, "fn") == 0) {
        token.type = TOKEN_KEYWORD_FN;
    } else if (strcmp(str, "pub") == 0) {
        token.type = TOKEN_KEYWORD_PUB;
    } else if (strcmp(str, "null") == 0) {
        token.type = TOKEN_KEYWORD_NULL;
    } else if (strcmp(str, "return") == 0) {
        token.type = TOKEN_KEYWORD_RETURN;
    } else if (strcmp(str, "void") == 0) {
        token.type = TOKEN_KEYWORD_VOID;
    } else if (strcmp(str, "i32") == 0) {
        token.type = TOKEN_KEYWORD_I32;
    } else if (strcmp(str, "?i32") == 0) {
        token.type = TOKEN_KEYWORD_I32_NULLABLE;
    } else if (strcmp(str, "f64") == 0) {
        token.type = TOKEN_KEYWORD_F64;
    } else if (strcmp(str, "?f64") == 0) {
        token.type = TOKEN_KEYWORD_F64_NULLABLE;
    } else if (strcmp(str, "u8") == 0) {
        token.type = TOKEN_KEYWORD_U8;
    } else if (strcmp(str, "[]u8") == 0) {
        token.type = TOKEN_KEYWORD_U8_ARRAY;
    } else if (strcmp(str, "?[]u8") == 0) {
        token.type = TOKEN_KEYWORD_U8_ARRAY_NULLABLE;
    } else {
        token.type = TOKEN_ERROR;  // key word undefined
    }
    return token;
}

bool isSeparator(char c) { 
    // Checking all possible separators
    if (strchr("+-*/=()[]{}<>&|!,;:", c) != NULL || isspace(c)) {   // Why is it red?
        return true;
    }
    return false;
}

// function to check if this is an identifier ##
int isIdentifier(const char* str) {
    regex_t regex;
    regcomp(&regex, "^[a-zA-Z_][a-zA-Z0-9_]*$", 0);  // Regular expression for identifiers ##
    int result = regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

// Function to check if it is a number ##
int isNumber(const char* str) {
    regex_t regex;
    regcomp(&regex, "^[0-9]+(\\.[0-9]+)?$", 0);  // Whole or fractional number ##
    int result = regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

// Function for processing lexemes ##
void processToken(const char* token) {
    // Use createToken
    if (isKeyword(token)) {
        printf("Keyword: %s\n", token); // Process keyword #
    } else if (isIdentifier(token)) {
        printf("Identifier: %s\n", token); // Process id #
    } else if (isDefineType(token)) {
        printf("Identifier: %s\n", token); // Process type i32, f64 etc. #
    } else if (isNumber(token)) {
        printf("Number: %s\n", token); // Process num #
    } else if (isSpecialSymbol(token)) {
        printf("Identifier: %s\n", token); // Process special symbol #
    } else {
        printf("Unknown token: %s\n", token); // Process Error #
    }
}

// The main function of the lexer ##
void lexer(const char* source_code) {
    LexerState state = STATE_NORMAL;  // Initial state
    char buffer[BUFFER_SIZE];  // Token Buffer
    int buffer_index = 0;  // Buffer index

    int i = 0;  // Index for source code symbols
    while (source_code[i] != '\0') {
        char c = source_code[i];
        // State handling
        switch (state) {
            case STATE_NORMAL:
                if (isSeparator(c)) { // a+b
                    if (buffer_index > 0) {
                        buffer[buffer_index] = '\0';  // put end of the buffer for future cmp
                        processToken(buffer);
                        buffer_index = 0;
                        // Check if it isn't whitespace and increase buffer otherwise
                        // Maybe add state AFTER_SPECIAL SIMBOL or process it right away
                    }
                } else if (c == '"') {  // String starts
                    if (buffer_index != 0){
                        buffer[buffer_index] = '\0';  // Process all from buffer
                        processToken(buffer);
                        buffer_index = 0;

                        state = STATE_STRING;   // Start String status 
                    }
                } else if (c == '/' && source_code[i + 1] == '/') {  // Comment starts
                    buffer[buffer_index] = '\0';  // Process all from buffer
                    processToken(buffer);
                    buffer_index = 0;

                    i++;    // Increasing i by 1, so next reading won't start from '\' 
                    state = STATE_COMMENT;
                    // Not sure if it's needed
                /*} else if (ispunct(c)) {  // TODO: MAYBE ADD CHECK 1 symb?
                    // Handling special character ##
                    if (buffer_index > 0) {
                        buffer[buffer_index] = '\0';
                        processToken(buffer);  // Token processing ##
                        buffer_index = 0;
                    }
                    printf("Special symbol: %c\n", c);*/
                } else {
                    buffer[buffer_index] = c;  // Add a character to the buffer ##
                    buffer_index++;
                }
                break;

            case STATE_STRING:
                if (c == '\\' && source_code[i + 1] == '"'){    // if '"' is part of the string    TODO: Can it raise an ERROR at the end? MAYBE ERROR
                    buffer[buffer_index] = c;
                    buffer_index++;
                    buffer[buffer_index] = '"';
                    buffer_index++;
                    i++;
                }else if (c == '"') { // end of the string
                    buffer[buffer_index] = '\0';
                    printf("String: \"%s\"\n", buffer);  // TODO: Gotta be another parser for str only
                    buffer_index = 0;
                    state = STATE_NORMAL;
                } else if (c == "\n"){
                    state = STATE_NEXT_LINE_STRING; // Starts next line
                } else {
                    buffer[buffer_index++] = c;  // String processing ##
                }
                break;
            case STATE_NEXT_LINE_STRING:
                if (isspace(c) == false){
                    if (c == '\\' && source_code[i + 1] == '\\'){   //TODO: Can it raise an ERROR at the end?
                        i++;
                        state = STATE_STRING;
                    }
                    else{
                        ; // TODO: ERROR OCCURE bc string isn't closed
                    }
                }
                break; 

            case STATE_COMMENT:
                if (c == '\n') {
                    state = STATE_NORMAL;   // Return to normal state after comment ##
                                            // Indexes must be zeros, so no changes needed
                }
                break;
        }
        i++;
    }

    // Processing the last token in the buffer ##
    if (buffer_index > 0 && state == STATE_NORMAL) {
        buffer[buffer_index] = '\0';
        processToken(buffer);
    }
    else {
        ; // TODO: ERROR OCCURE
    }
}

int main() {
    const char* code = "fn main() { const x = 42; // This is a comment\n print(\"Hello, World!\"); }";
    lexer(code);
    return 0;
}


int isSpecialSymbol(char c) {
    return strchr("+-*/=()[]{}", c) != NULL;
}

// Not much used now, but will be in the near future
token_t processSpecialSymbol(char c) {
    token_t token;
    switch (c) {
        // Math Equations
        case '=':
            token.type = TOKEN_ASSIGNMENT;
            break;
        case '+':
            token.type = TOKEN_ADDITION;
            break;
        case '-':
            token.type = TOKEN_SUBTRACTION;
            break;
        case '*':
            token.type = TOKEN_MULTIPLICATION;
            break;
        case '/':
            token.type = TOKEN_DIVISION;
            break;
        // Brackets
        case '(':
            token.type = TOKEN_LEFT_ROUND_BRACKET;
            break;
        case ')':
            token.type = TOKEN_RIGHT_ROUND_BRACKET;
            break;
        case '[':
            token.type = TOKEN_LEFT_SQUARE_BRACKET;
            break;
        case ']':
            token.type = TOKEN_RIGHT_SQUARE_BRACKET;
            break;
        case '{':
            token.type = TOKEN_LEFT_CURLY_BRACKET;
            break;
        case '}':
            token.type = TOKEN_RIGHT_CURLY_BRACKET;
            break;
        // Special symbols
        case '@':
            token.type = TOKEN_AT;
            break;
        case ';':
            token.type = TOKEN_SEMICOLON;
            break;
        case ',':
            token.type = TOKEN_COMMA;
            break;
        case '.':
            token.type = TOKEN_DOT;
            break;
        /*
        case '..':
            token.type = TOKEN_RANGE;   // If needed would be proceeded in another form
            break;
        */
        case ':':
            token.type = TOKEN_COLON;
            break;
        default:
            token.type = TOKEN_ERROR;
            break;
    }
    return token;
}

// Is it a num TODO: more characteristics
int isNumber(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;  // Err
        }
    }
    return 1;
}
