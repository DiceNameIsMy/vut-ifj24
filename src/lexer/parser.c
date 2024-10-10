// Created by Savin Ivan
// I think my implementation is too different from Nur's
// So I won't do anything with it

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

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
    "const", "var", "if", "else", "while", "fn", "pub", 
    "null", "return", "void", 
    "i32", "?i32", "f64", "?f64", "u8", "[]u8", "?[]u8"
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

token_type_t processKeyword(const char* str) {
    token_type_t type = TOKEN_ERROR;;

    if (strcmp(str, "const") == 0) {
        type = TOKEN_KEYWORD_CONST;
    } else if (strcmp(str, "var") == 0) {
        type = TOKEN_KEYWORD_VAR;
    } else if (strcmp(str, "if") == 0) {
        type = TOKEN_KEYWORD_IF;
    } else if (strcmp(str, "else") == 0) {
        type = TOKEN_KEYWORD_ELSE;
    } else if (strcmp(str, "while") == 0) {
        type = TOKEN_KEYWORD_WHILE;
    } else if (strcmp(str, "fn") == 0) {
        type = TOKEN_KEYWORD_FN;
    } else if (strcmp(str, "pub") == 0) {
        type = TOKEN_KEYWORD_PUB;
    } else if (strcmp(str, "null") == 0) {
        type = TOKEN_KEYWORD_NULL;
    } else if (strcmp(str, "return") == 0) {
        type = TOKEN_KEYWORD_RETURN;
    } else if (strcmp(str, "void") == 0) {
        type = TOKEN_KEYWORD_VOID;
    } else if (strcmp(str, "i32") == 0) {
        type = TOKEN_KEYWORD_I32;
    } else if (strcmp(str, "?i32") == 0) {
        type = TOKEN_KEYWORD_I32_NULLABLE;
    } else if (strcmp(str, "f64") == 0) {
        type = TOKEN_KEYWORD_F64;
    } else if (strcmp(str, "?f64") == 0) {
        type = TOKEN_KEYWORD_F64_NULLABLE;
    } else if (strcmp(str, "u8") == 0) {
        type = TOKEN_KEYWORD_U8;
    } else if (strcmp(str, "[]u8") == 0) {
        type = TOKEN_KEYWORD_U8_ARRAY;
    } else if (strcmp(str, "?[]u8") == 0) {
        type = TOKEN_KEYWORD_U8_ARRAY_NULLABLE;
    }
    return type;
}

bool isSeparator(char c) { 
    // Checking all possible separators
    if (strchr("+-*/=()[]{}<>&|!,;:", c) != NULL || isspace(c)) {   // Why is it red?
        return true;
    }
    return false;
}

// function to check if this is an identifier
bool isIdentifier(const char* str) {
    regex_t regex;
    // Regular expression that checks if string is a valid identifier
    // but disallows a single underscore '_'
    if (regcomp(&regex, "^[a-zA-Z][a-zA-Z0-9_]*$|^[a-zA-Z_][a-zA-Z0-9_]+$", REG_EXTENDED)) {
        return false;  // Return false if regex compilation fails
    }
    // Execute the regex check
    int result = regexec(&regex, str, 0, NULL, 0);
    // Free the regex
    regfree(&regex);
    // Return true if the regex matched the string, otherwise false
    return result == 0;
}

int identifyNumberType(const char* str) {
    regex_t i32_regex, f64_regex;
    
    // i32 literal regex
    const char* i32_pattern = "^[0-9]+$";
    // f64 literal regex with number before '.'
    const char* f64_pattern = "^([0-9]+\\.[0-9]*([eE][+-]?[0-9]+)?)|(\\.[0-9]+([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+)$";
    
    // Compile regex
    regcomp(&i32_regex, i32_pattern, REG_EXTENDED);
    regcomp(&f64_regex, f64_pattern, REG_EXTENDED);

    // Check if string matches i32 pattern
    if (regexec(&i32_regex, str, 0, NULL, 0) == 0) {
        regfree(&i32_regex);
        regfree(&f64_regex);
        return 1;  // It's an i32 literal
    }

    // Check if string matches f64 pattern
    if (regexec(&f64_regex, str, 0, NULL, 0) == 0) {
        regfree(&i32_regex);
        regfree(&f64_regex);
        return 2;  // It's an f64 literal
    }

    // Free regex memory
    regfree(&i32_regex);
    regfree(&f64_regex);
    
    return 0;  // Neither i32 nor f64 literal
}

bool isSpecialSymbol(char c) {
    return strchr("+-*/=()[]{}", c) != NULL;
}

// Not much used now, but will be in the near future
token_type_t processSpecialSymbol(char c) {
    token_type_t type;
    switch (c) {
        // Math Equations
        case '=':
            type = TOKEN_ASSIGNMENT;
            break;
        case '+':
            type = TOKEN_ADDITION;
            break;
        case '-':
            type = TOKEN_SUBTRACTION;
            break;
        case '*':
            type = TOKEN_MULTIPLICATION;
            break;
        case '/':
            type = TOKEN_DIVISION;
            break;
        // Brackets
        case '(':
            type = TOKEN_LEFT_ROUND_BRACKET;
            break;
        case ')':
            type = TOKEN_RIGHT_ROUND_BRACKET;
            break;
        case '[':
            type = TOKEN_LEFT_SQUARE_BRACKET;
            break;
        case ']':
            type = TOKEN_RIGHT_SQUARE_BRACKET;
            break;
        case '{':
            type = TOKEN_LEFT_CURLY_BRACKET;
            break;
        case '}':
            type = TOKEN_RIGHT_CURLY_BRACKET;
            break;
        // Special symbols
        case '@':
            type = TOKEN_AT;
            break;
        case ';':
            type = TOKEN_SEMICOLON;
            break;
        case ',':
            type = TOKEN_COMMA;
            break;
        case '.':
            type = TOKEN_DOT;
            break;
        /*
        case '..':
            token.type = TOKEN_RANGE;   // If needed would be proceeded in another form
            break;
        */
        case ':':
            type = TOKEN_COLON;
            break;
        default:
            type = TOKEN_ERROR;
            break;
    }
    return type;
}


// Function for processing lexemes
void processToken(const char* buf_str) {
    token_type_t token_type;
    token_attribute attribute;
    attribute.str = NULL;

    if (isKeyword(buf_str)) {
        token_type = processKeyword(buf_str);
        printf("Keyword: %s\n", buf_str); // Process keyword and types i32, f64 etc.
    } 
    else if (isIdentifier(buf_str)) {
        token_type = TOKEN_ID;
        printf("Identifier: %s\n", buf_str); // Process id
    }
    else if (identifyNumberType(buf_str)) {     // != 0
        if (identifyNumberType(buf_str) == 1){
            token_type = TOKEN_I32_LITERAL;
            attribute.integer = atoi(buf_str);
        }
        if (identifyNumberType(buf_str) == 2){
            token_type = TOKEN_F64_LITERAL;
            attribute.real = strtod(buf_str, NULL);
        }
        printf("Number: %s\n", buf_str); // Process num
    } 
    else if (isSpecialSymbol(buf_str) && buf_str[1] == '\0') {
        token_type = processSpecialSymbol(buf_str[0]);
        printf("Special symbol: %s\n", buf_str); // Process special symbol
    } else {
        printf("Unknown token: %s\n", buf_str); // Unxpected input, Error
        exit(1);
    }
    token_t token = createToken(buf_str, attribute); 
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