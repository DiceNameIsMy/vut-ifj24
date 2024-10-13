#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef enum {
    STATE_NORMAL,
    STATE_STRING,
    STATE_NEXT_LINE_STRING,
    STATE_COMMENT,
} LexerState;  // FSM which decides, how we aproach characters


// Array of keywords
const char* keywords[] = {
    "const", "var", "if", "else", "while", "fn", "pub",
    "null", "return", "void",
    "i32", "?i32", "f64", "?f64", "u8", "[]u8", "?[]u8"
};

bool isKeyword(const char* str);
token_type_t processKeyword(const char* str);
bool isSeparator(char c);
bool isIdentifier(const char* str);
int identifyNumberType(const char* str);
bool isSpecialSymbol(char c);
token_type_t processSpecialSymbol(char c);
void processToken(const char* buf_str);
void lexer(const char* source_code);
#endif // LEXER_H
