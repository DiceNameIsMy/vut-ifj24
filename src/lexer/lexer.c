// Created by Savin Ivan
// I think my implementation is too different from Nur's
// So I won't do anything with it

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "structs/dynBuffer.h"
#include "token.c"
#include "logging.h"

typedef enum {
    STATE_COMMON,
    STATE_ONE_LINE_STRING,
    STATE_NEXT_LINE_STRING,
    STATE_COMMENT,
} LexerState; // FSM which decides, how we approach characters

// Array of keywords
const char *keywords[] = {
    "const", "var", "if", "else", "while", "fn", "pub",
    "null", "return", "void",
    "i32", "?i32", "f64", "?f64", "u8", "[]u8", "?[]u8"
};

bool isKeyword(const char *str);

TokenType processKeyword(const char *str);

bool isSeparator(char c);

bool isIdentifier(const char *str);

int identifyNumberType(const char *str);

bool isSpecialSymbol(char c);

TokenType processSpecialSymbol(char c);

void processToken(const char *buf_str, TokenArray *array);

// Token reader Buffer size
#define BUFFER_SIZE 256

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))   // Amount of key words

// checking is it a key word
bool isKeyword(const char *str) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true; // Found
        }
    }
    return false; // Not found
}

TokenType processKeyword(const char *str) {
    TokenType type = TOKEN_ERROR;;

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

// function to check if this is an identifier
bool isIdentifier(const char *str) {
    regex_t regex;
    // Regular expression that checks if string is a valid identifier
    // but disallows a single underscore '_'
    if (regcomp(&regex, "^[a-zA-Z_][a-zA-Z0-9_]*$", REG_EXTENDED)) {
        return false; // Return false if regex compilation fails
    }
    // Execute the regex check
    int result = regexec(&regex, str, 0, NULL, 0);
    // Free the regex
    regfree(&regex);

    // check not solo _
    if (result == 0 && strcmp(str, "_") == 0) {
        return false;
    }
    // Return true if the regex matched the string, otherwise false
    return result == 0;
}

int identifyNumberType(const char *str) {
    regex_t i32_regex, f64_regex;

    // i32 literal regex
    const char *i32_pattern = "^[0-9]+$";
    // f64 literal regex with number before '.'
    const char *f64_pattern =
            "^([0-9]+\\.[0-9]*([eE][+-]?[0-9]+)?)|(\\.[0-9]+([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+)$";

    // Compile regex
    regcomp(&i32_regex, i32_pattern, REG_EXTENDED);
    regcomp(&f64_regex, f64_pattern, REG_EXTENDED);

    // Check if string matches i32 pattern
    if (regexec(&i32_regex, str, 0, NULL, 0) == 0) {
        regfree(&i32_regex);
        regfree(&f64_regex);
        return 1; // It's an i32 literal
    }

    // Check if string matches f64 pattern
    if (regexec(&f64_regex, str, 0, NULL, 0) == 0) {
        regfree(&i32_regex);
        regfree(&f64_regex);
        return 2; // It's an f64 literal
    }

    // Free regex memory
    regfree(&i32_regex);
    regfree(&f64_regex);

    return 0; // Neither i32 nor f64 literal
}

bool isSpecialSymbol(char c) {
    return strchr("+-*/=()[]{};", c) != NULL;
}

// Not much used now, but will be in the near future
TokenType processSpecialSymbol(char c) {
    TokenType type;
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
void processToken(const char *buf_str, TokenArray *array) {
    TokenType token_type;
    TokenAttribute attribute;
    attribute.str = NULL;

    if (isKeyword(buf_str)) {
        token_type = processKeyword(buf_str);
        printf("Keyword: %s\n", buf_str); // Process keyword and types i32, f64 etc.
    } else if (isIdentifier(buf_str)) {
        token_type = TOKEN_ID;
        printf("Identifier: %s\n", buf_str); // Process id
        attribute.str = strdup(buf_str); // copy

        if (attribute.str == NULL) {
            fprintf(stderr, "Error memory allocation\n");
            exit(0); // TODO: clean
        }
    } else if (identifyNumberType(buf_str)) {
        // != 0
        if (identifyNumberType(buf_str) == 1) {
            token_type = TOKEN_I32_LITERAL;
            attribute.integer = atoi(buf_str);
        }
        if (identifyNumberType(buf_str) == 2) {
            token_type = TOKEN_F64_LITERAL;
            attribute.real = strtod(buf_str, NULL);
        }
        printf("Number: %s\n", buf_str); // Process num
    } else if (isSpecialSymbol(buf_str[0]) && buf_str[1] == '\0') {
        token_type = processSpecialSymbol(buf_str[0]);
        printf("Special symbol: %s\n", buf_str); // Process special symbol
    } else {
        printf("Unknown token: %s\n", buf_str); // Unxpected input, Error TODO?
        printf("Possible ERROR!\n\n");
        //exit(1);
    }
    Token token = createToken(token_type, attribute);
    addToken(array, token);
}

bool isSeparator(char c) {
    // Checking all possible separators
    if (strchr("+-*/=()[]{}<>&|!,;:", c) != NULL || isspace(c)) {
        // Why is it red?
        return true;
    }
    return false;
}

LexerState fsmParseOnCommonState(const char *sourceCode, int *i, TokenArray *tokenArray, DynBuffer *buff) {
    LexerState nextState = STATE_COMMON;
    const char c = sourceCode[*i];

    const bool bufferIsEmpty = isDynBufferEmpty(buff);

    if (isSeparator(c)) {
        if (!bufferIsEmpty) {
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }

        const bool soloSymbol = strchr("+-*/=()[]{}&|,;:", c) != NULL;
        if (soloSymbol) {
            // Todo -> few rows above
            appendDynBuffer(buff, c); // TODO: ==, >=, <=, !=, check all symbols
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        // Check if it isn't whitespace and increase buffer otherwise
        // Maybe add state AFTER_SPECIAL SIMBOL or process it right away
    } else if (c == '"') {
        // String starts
        if (!bufferIsEmpty) {
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        nextState = STATE_ONE_LINE_STRING; // Start String status
    } else if (c == '/' && sourceCode[*i + 1] == '/') {
        // Comment starts
        if (!bufferIsEmpty) {
            // TODO: MAKE IT A FUNC
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        (*i)++; // Increasing i by 1, so next reading won't start from the second '/'
        nextState = STATE_COMMENT;
    } else {
        appendDynBuffer(buff, c);
    }
    return nextState;
}

LexerState fsmStepOnOneLineStringParsing(const char *sourceCode, int *i, TokenArray *tokenArray, DynBuffer *buff) {
    LexerState nextState = STATE_ONE_LINE_STRING;
    const char c = sourceCode[*i];

    if (c == '\\' && sourceCode[*i + 1] == '"') {
        // if '"' is part of the string    TODO: Can it raise an ERROR at the end? MAYBE ERROR
        appendDynBuffer(buff, '"');
        (*i)++;
    } else if (c == '"') {
        // end of the string
        printf("String: \"%s\"\n", buff->data); // TODO: Gotta be another parser for str only

        Token stringToken = {.type = TOKEN_STRING_LITERAL};
        initStringAttribute(&stringToken.attribute, buff->data);
        addToken(tokenArray, stringToken);

        emptyDynBuffer(buff);
        nextState = STATE_COMMON;
    } else if (c == '\n') {
        // Put an error since double quote strings can't be multiline
        Token errorToken = {.type = TOKEN_ERROR};
        initStringAttribute(&errorToken.attribute, "Got \\n whilst parsing a double quote string");
        addToken(tokenArray, errorToken);
        emptyDynBuffer(buff);
        nextState = STATE_COMMON;
    } else {
        appendDynBuffer(buff, c);
    }

    return nextState;
}


// The main function of the lexer ##
void runLexer(const char *sourceCode, TokenArray *tokenArray) {
    LexerState state = STATE_COMMON; // Initial state

    DynBuffer buff;
    initDynBuffer(&buff, -1);

    int i = 0; // Index for source code symbols
    while (sourceCode[i] != '\0') {
        const char c = sourceCode[i];
        // State handling
        switch (state) {
            case STATE_COMMON:
                state = fsmParseOnCommonState(sourceCode, &i, tokenArray, &buff);
                break;

            case STATE_ONE_LINE_STRING:
                state = fsmStepOnOneLineStringParsing(sourceCode, &i, tokenArray, &buff);
                break;

            case STATE_NEXT_LINE_STRING:
                if (isspace(c) == false) {
                    if (c == '\\' && sourceCode[i + 1] == '\\') {
                        //TODO: Can it raise an ERROR at the end?
                        i++;
                        state = STATE_ONE_LINE_STRING;
                    } else {
                        ; // TODO: ERROR OCCURE bc string isn't closed
                    }
                }
                break;

            case STATE_COMMENT:
                if (c == '\n') {
                    state = STATE_COMMON; // Return to normal state after comment ##
                    // Indexes must be zeros, so no changes needed
                }
                break;
        }
        i++;
    }

    // Processing the last token in the buffer ##
    if (!isDynBufferEmpty(&buff)) {
        if (state == STATE_COMMON) {
            processToken(buff.data, tokenArray);
        } else if (state == STATE_ONE_LINE_STRING) {
            Token errorToken = {.type = TOKEN_ERROR};
            initStringAttribute(&errorToken.attribute, "Got \\n whilst parsing a double quote string");
            addToken(tokenArray, errorToken);
        }
        emptyDynBuffer(&buff);
    } else {
        ; // TODO: ERROR OCCURED
    }
}
