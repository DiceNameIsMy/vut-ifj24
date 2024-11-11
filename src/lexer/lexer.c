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

typedef enum {
    STATE_COMMON,
    STATE_ONE_LINE_STRING,
    STATE_MULTILINE_STRING,
    STATE_MULTILINE_STRING_SKIP_WHITESPACE,
    STATE_COMMENT,
} LexerState; // FSM which decides, how we approach characters

// Array of keywords
const char *keywords[] = {
    "const", "var", "if", "else", "while", "fn", "pub",
    "null", "return", "void", "@import",
    "i32", "?i32", "f64", "?f64", "u8", "[]u8", "?[]u8"
};

// If str is one of keywords, set keywordType value to the proper token type and return true.
// False otherwise.
bool tryGetKeyword(const char *str, TokenType *keywordType, TokenArray *array);

bool tryGetI32(const char *str, int *i32);

bool tryGetF64(const char *str, double *f64);

bool tryGetSymbol(const char *str, TokenType *tokenType);

bool isSeparator(char c);

bool isIdentifier(const char *str);

void processToken(const char *buf_str, TokenArray *array);

void processTokenI32(TokenType *keywordType, TokenArray *array);

void processTokenF64(TokenType *keywordType, TokenArray *array);

void processTokenU8(TokenType *keywordType, TokenArray *array);

// Token reader Buffer size
#define BUFFER_SIZE 256

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))   // Amount of keywords

bool tryGetKeyword(const char *str, TokenType *keywordType, TokenArray *array) {
    if (strcmp(str, "const") == 0) {
        *keywordType = TOKEN_KEYWORD_CONST;
    } else if (strcmp(str, "var") == 0) {
        *keywordType = TOKEN_KEYWORD_VAR;
    } else if (strcmp(str, "if") == 0) {
        *keywordType = TOKEN_KEYWORD_IF;
    } else if (strcmp(str, "else") == 0) {
        *keywordType = TOKEN_KEYWORD_ELSE;
    } else if (strcmp(str, "while") == 0) {
        *keywordType = TOKEN_KEYWORD_WHILE;
    } else if (strcmp(str, "fn") == 0) {
        *keywordType = TOKEN_KEYWORD_FN;
    } else if (strcmp(str, "pub") == 0) {
        *keywordType = TOKEN_KEYWORD_PUB;
    } else if (strcmp(str, "null") == 0) {
        *keywordType = TOKEN_KEYWORD_NULL;
    } else if (strcmp(str, "return") == 0) {
        *keywordType = TOKEN_KEYWORD_RETURN;
    } else if (strcmp(str, "void") == 0) {
        *keywordType = TOKEN_KEYWORD_VOID;
    } else if (strcmp(str, "@import") == 0) {
        *keywordType = TOKEN_KEYWORD_IMPORT;
    } else if (strcmp(str, "i32") == 0) {
        processTokenI32(keywordType, array);
    } else if (strcmp(str, "f64") == 0) {
        processTokenF64(keywordType, array);
    } else if (strcmp(str, "u8") == 0) {
        processTokenU8(keywordType, array);
    } else {
        return false;
    }
    return true;
}

void processTokenI32(TokenType *keywordType, TokenArray *array) {
    // ?i32 case
    if (array->size >= 1 &&
        array->tokens[array->size - 1].type == TOKEN_QUESTION_MARK) {
        deleteLastToken(array);
        *keywordType = TOKEN_KEYWORD_I32_NULLABLE;
        loginfo("remaking into ?i32");
    }
    // i32 case
    else {
        *keywordType = TOKEN_KEYWORD_I32;
    }
}

void processTokenF64(TokenType *keywordType, TokenArray *array) {
    if (array->size >= 1)
        // ?f64 case
        if (array->tokens[array->size - 1].type == TOKEN_QUESTION_MARK) {
            deleteLastToken(array);
            *keywordType = TOKEN_KEYWORD_F64_NULLABLE;

            loginfo("remaking into ?f64");
        }
        // f64 case
        else {
            *keywordType = TOKEN_KEYWORD_F64;
        }
}

void processTokenU8(TokenType *keywordType, TokenArray *array) {
    // ?[]u8 case
    if (array->size >= 3 &&
        array->tokens[array->size - 3].type == TOKEN_QUESTION_MARK &&
        array->tokens[array->size - 2].type == TOKEN_LEFT_SQUARE_BRACKET &&
        array->tokens[array->size - 1].type == TOKEN_RIGHT_SQUARE_BRACKET) {
        deleteLastToken(array);
        deleteLastToken(array);
        deleteLastToken(array);
        *keywordType = TOKEN_KEYWORD_U8_ARRAY_NULLABLE;
        loginfo("remaking into ?[]u8");
    }
    // []u8 case
    else if (array->size >= 2 &&
             array->tokens[array->size - 2].type == TOKEN_LEFT_SQUARE_BRACKET &&
             array->tokens[array->size - 1].type == TOKEN_RIGHT_SQUARE_BRACKET) {
        deleteLastToken(array);
        deleteLastToken(array);
        *keywordType = TOKEN_KEYWORD_U8_ARRAY;
        loginfo("remaking into []u8");
    }
    // u8 case
    else {
        *keywordType = TOKEN_KEYWORD_U8;
    }
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

bool tryGetI32(const char *str, int *i32) {
    const char *i32_pattern = "^[0-9]+$";
    regex_t i32_regex;
    regcomp(&i32_regex, i32_pattern, REG_EXTENDED);

    const bool match = regexec(&i32_regex, str, 0, NULL, 0) == 0;
    regfree(&i32_regex);

    if (match) {
        // i32 can not start with leading zeros
        if (str[0] == '0' && strlen(str) > 1) {
            return false;
        }

        *i32 = (int) strtol(str, NULL, 10);
        return true;
    }
    return false;
}

bool isF64(const char *str) {
    return false;
}

bool tryGetF64(const char *str, double *f64) {
    const char *f64_pattern =
            "^([0-9]+\\.[0-9]*([eE][+-]?[0-9]+)?)|(\\.[0-9]+([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+)$";
    regex_t f64_regex;
    regcomp(&f64_regex, f64_pattern, REG_EXTENDED);

    const bool match = regexec(&f64_regex, str, 0, NULL, 0) == 0;
    regfree(&f64_regex);

    if (match) {
        // f64 can not start with leading zeros
        const bool firstIsZero = str[0] == '0';
        const bool secondIsNumber = str[1] >= '0' && str[1] <= '9';
        if (firstIsZero && secondIsNumber) {
            return false;
        }

        if (f64 != NULL) {
            *f64 = strtod(str, NULL);
        }
        return true;
    }
    return false;
}


bool tryGetSymbol(const char *str, TokenType *tokenType) {
    if (strcmp(str, "=") == 0) {
        *tokenType = TOKEN_ASSIGNMENT;
    } else if (strcmp(str, "+") == 0) {
        // Operations
        *tokenType = TOKEN_ADDITION;
    } else if (strcmp(str, "-") == 0) {
        *tokenType = TOKEN_SUBTRACTION;
    } else if (strcmp(str, "*") == 0) {
        *tokenType = TOKEN_MULTIPLICATION;
    } else if (strcmp(str, "/") == 0) {
        *tokenType = TOKEN_DIVISION;
    } else if (strcmp(str, "(") == 0) {
        // Brackets
        *tokenType = TOKEN_LEFT_ROUND_BRACKET;
    } else if (strcmp(str, ")") == 0) {
        *tokenType = TOKEN_RIGHT_ROUND_BRACKET;
    } else if (strcmp(str, "[") == 0) {
        *tokenType = TOKEN_LEFT_SQUARE_BRACKET;
    } else if (strcmp(str, "]") == 0) {
        *tokenType = TOKEN_RIGHT_SQUARE_BRACKET;
    } else if (strcmp(str, "{") == 0) {
        *tokenType = TOKEN_LEFT_CURLY_BRACKET;
    } else if (strcmp(str, "}") == 0) {
        *tokenType = TOKEN_RIGHT_CURLY_BRACKET;
    } else if (strcmp(str, "|") == 0) {
        *tokenType = TOKEN_VERTICAL_BAR;
    } else if (strcmp(str, "<") == 0) {
        // Comparison operators
        *tokenType = TOKEN_LESS_THAN;
    } else if (strcmp(str, "<=") == 0) {
        *tokenType = TOKEN_LESS_THAN_OR_EQUAL_TO;
    } else if (strcmp(str, ">") == 0) {
        *tokenType = TOKEN_GREATER_THAN;
    } else if (strcmp(str, ">=") == 0) {
        *tokenType = TOKEN_GREATER_THAN_OR_EQUAL_TO;
    } else if (strcmp(str, "==") == 0) {
        *tokenType = TOKEN_EQUAL_TO;
    } else if (strcmp(str, "!=") == 0) {
        *tokenType = TOKEN_NOT_EQUAL_TO;
    } else if (strcmp(str, ";") == 0) {
        *tokenType = TOKEN_SEMICOLON;
    } else if (strcmp(str, ",") == 0) {
        *tokenType = TOKEN_COMMA;
    } else if (strcmp(str, ".") == 0) {
        *tokenType = TOKEN_DOT;
    } else if (strcmp(str, ":") == 0) {
        *tokenType = TOKEN_COLON;
    } else if (strcmp(str, "?") == 0) {
        *tokenType = TOKEN_QUESTION_MARK;
    } else {
        return false;
    }
    return true;
}


// Function for processing lexemes
void processToken(const char *buf_str, TokenArray *array) {
    TokenType tokenType;
    TokenAttribute attribute;

    if (tryGetKeyword(buf_str, &tokenType, array)) {
        loginfo("Keyword: %s\n", buf_str); // Process keyword and types i32, f64 etc.
    } else if (isIdentifier(buf_str)) {
        tokenType = TOKEN_ID;
        loginfo("Identifier: %s\n", buf_str); // Process id
        attribute.str = strdup(buf_str); // copy

        if (attribute.str == NULL) {
            loginfo("Error memory allocation\n");
            exit(0); // TODO: clean
        }
    } else if (tryGetI32(buf_str, &attribute.integer)) {
        tokenType = TOKEN_I32_LITERAL;
        loginfo("Integer number: %s\n", buf_str); // Process num
    } else if (tryGetF64(buf_str, &attribute.real)) {
        tokenType = TOKEN_F64_LITERAL;
        loginfo("Float number: %s\n", buf_str); // Process num
    } else if (tryGetSymbol(buf_str, &tokenType)) {
        loginfo("Special symbol: %s\n", buf_str); // Process special symbol
    } else {
        loginfo("Unknown token: %s\n", buf_str); // Unexpected input, Error TODO?
        loginfo("Possible ERROR!\n\n");
        tokenType = TOKEN_ERROR;
        attribute.str = "Unrecognized token";
    }
    const Token token = createToken(tokenType, attribute);
    addToken(array, token);
}

bool isSeparator(char c) {
    // Checking all possible separators
    if (strchr("+-*=()[]{}<>&|!,;:?", c) != NULL || isspace(c)) {
        return true;
    }
    return false;
}

bool isPairedSymbol(char c, char c_next) {
    if ((c == '=' && c_next == '=') ||
        (c == '!' && c_next == '=') ||
        (c == '>' && c_next == '=') ||
        (c == '<' && c_next == '=') ||
        (c == '&' && c_next == '&') ||
        (c == '|' && c_next == '|')) {
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
            // In case of 0.2E-2 or 0.1e+1

            const bool is_f64 = (c == '-' || c == '+')
                && tolower(sourceCode[(*i) - 1]) == 'e'
                && tryGetF64(buff->data, NULL);
            if (is_f64) {

            }
            else {
                processToken(buff->data, tokenArray);
                emptyDynBuffer(buff);
            }
        }
        const bool soloSymbol = strchr("+-*=()[]{}&|,;:?", c) != NULL;
        // Check for == >= <= != || &&
        if (isPairedSymbol(c, sourceCode[(*i) + 1])) {
            appendDynBuffer(buff, c);
            (*i)++; // skip next symbol
            appendDynBuffer(buff, sourceCode[*i]);
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        } else if (soloSymbol) {
            // In case of 0.2E-2 or 0.1e+1
            if ((c == '-' || c == '+') && tolower(sourceCode[(*i) - 1]) == 'e' && tryGetF64(buff->data, NULL)) {
                appendDynBuffer(buff, c);
            } else {
                // normal solo symbol
                appendDynBuffer(buff, c);
                processToken(buff->data, tokenArray);
                emptyDynBuffer(buff);
            }
        }
        // Check if it isn't whitespace and increase buffer otherwise
        // Maybe add state AFTER_SPECIAL SYMBOL or process it right away
    } else if (c == '"') {
        // String starts
        if (!bufferIsEmpty) {
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        nextState = STATE_ONE_LINE_STRING; // Start String status
    } else if (c == '/') {
        // Process buffer data
        if (!bufferIsEmpty) {
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        if (sourceCode[*i + 1] == '/') {
            (*i)++; // Increasing i by 1, so next reading won't start from the second '/'
            nextState = STATE_COMMENT;
        } else {
            appendDynBuffer(buff, c);
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
    }  else if (c == '\\') {
        // Process buffer data
        if (!bufferIsEmpty) {
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
        if (sourceCode[*i + 1] == '\\') {
            (*i)++; // Increasing i by 1, so next reading won't start from the second '/'
            nextState = STATE_MULTILINE_STRING;
        } else {
            appendDynBuffer(buff, c);
            processToken(buff->data, tokenArray);
            emptyDynBuffer(buff);
        }
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
        loginfo("String: \"%s\"\n", buff->data); // TODO: Gotta be another parser for str only

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

            case STATE_MULTILINE_STRING:
                if (c == '\n') {
                    state = STATE_MULTILINE_STRING_SKIP_WHITESPACE;
                } else {
                    appendDynBuffer(&buff, c);
                }
                break;

            case STATE_MULTILINE_STRING_SKIP_WHITESPACE:
                const bool isWhitespace = c == ' ' || c == '\t';
                if (isWhitespace) {
                    break;
                }
                if (c == ';') {
                    Token stringToken = {.type = TOKEN_STRING_LITERAL};
                    initStringAttribute(&stringToken.attribute, buff.data);
                    addToken(tokenArray, stringToken);

                    emptyDynBuffer(&buff);

                    const Token semicolonToken = {.type = TOKEN_SEMICOLON};
                    addToken(tokenArray, semicolonToken);

                    state = STATE_COMMON;

                    break;
                }
                if (c == '\\') {
                    if (sourceCode[i + 1] == '\\') {
                        // Multiline string continues -> a newline for that must be added
                        appendDynBuffer(&buff, '\n');
                        state = STATE_MULTILINE_STRING;
                    } else {
                        Token errorToken = {.type = TOKEN_ERROR};
                        initStringAttribute(&errorToken.attribute, "Multiline string literal was not ended properly");
                        addToken(tokenArray, errorToken);
                        state = STATE_COMMON;
                    }
                    i++;
                } else {
                    Token errorToken = {.type = TOKEN_ERROR};
                    initStringAttribute(&errorToken.attribute, "Multiline string literal was not ended properly");
                    addToken(tokenArray, errorToken);
                    state = STATE_COMMON;
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
            initStringAttribute(&errorToken.attribute, "One line string literal was not ended");
            addToken(tokenArray, errorToken);
        }
        emptyDynBuffer(&buff);
    } else {
        ; // TODO: ERROR OCCURRED
    }
}
