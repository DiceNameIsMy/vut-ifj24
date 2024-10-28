#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>

typedef enum {
    TOKEN_ID, // An identifier. It's value can be found at attribute->str
    // Keywords
    TOKEN_KEYWORD_CONST,
    TOKEN_KEYWORD_VAR,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_FN,
    TOKEN_KEYWORD_PUB,
    TOKEN_KEYWORD_NULL,
    TOKEN_KEYWORD_RETURN,
    TOKEN_KEYWORD_VOID,
    TOKEN_KEYWORD_IMPORT, // @import
    // Data types
    TOKEN_KEYWORD_I32, // i32
    TOKEN_KEYWORD_I32_NULLABLE, // ?i32
    TOKEN_KEYWORD_F64, // f64
    TOKEN_KEYWORD_F64_NULLABLE, // ?f64
    TOKEN_KEYWORD_U8, // u8
    TOKEN_KEYWORD_U8_ARRAY, // []u8
    TOKEN_KEYWORD_U8_ARRAY_NULLABLE, // ?[]u8
    // Literals
    TOKEN_STRING_LITERAL, // "string"
    TOKEN_I32_LITERAL, // 32
    TOKEN_F64_LITERAL, // 0.64
    // Operations
    TOKEN_ASSIGNMENT, // =
    TOKEN_ADDITION, // +
    TOKEN_SUBTRACTION, // -
    TOKEN_MULTIPLICATION, // *
    TOKEN_DIVISION, // /
    // Comparisons
    TOKEN_LESS_THAN, // <
    TOKEN_LESS_THAN_OR_EQUAL_TO, // <=
    TOKEN_GREATER_THAN, // >
    TOKEN_GREATER_THAN_OR_EQUAL_TO, // >=
    TOKEN_EQUAL_TO, // ==
    TOKEN_NOT_EQUAL_TO, // !=
    // Brackets
    TOKEN_LEFT_ROUND_BRACKET, // (
    TOKEN_RIGHT_ROUND_BRACKET, // )
    TOKEN_LEFT_SQUARE_BRACKET, // [
    TOKEN_RIGHT_SQUARE_BRACKET, // ]
    TOKEN_LEFT_CURLY_BRACKET, // {
    TOKEN_RIGHT_CURLY_BRACKET, // }
    TOKEN_VERTICAL_BAR, // |
    // Special symbols
    TOKEN_QUESTIONMARK,
    TOKEN_SEMICOLON, // ;
    TOKEN_COMMA, // ,
    TOKEN_DOT, // .
    TOKEN_COLON, // :
    TOKEN_ERROR // Something went wrong. Message could be included in the attributes.
    // TODO: any other cases?
} TokenType;

typedef union {
    char *str; // Could be changed to be different structure with info about length and allocated memory
    int integer;
    double real;
} TokenAttribute; // Made almost fully for variables and for syntax analysis

typedef struct {
    TokenType type;
    TokenAttribute attribute;
} Token;

typedef struct {
    Token *tokens;
    size_t size; // Amount of tokens in the Array now
    size_t capacity;
} TokenArray;

int initTokenArray(TokenArray *array);

void freeTokenArray(TokenArray *array);

int addToken(TokenArray *array, Token token);

Token createToken(const TokenType type, const TokenAttribute attribute);

int initStringAttribute(TokenAttribute *attr, char *str);

void freeToken(Token *token);

#endif // TOKEN_H
