#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "lexer/token.h"

#define TOKEN_ARRAY_INIT_CAPACITY 16

int initTokenArray(TokenArray *array) {
    array->tokens = (Token *) malloc(TOKEN_ARRAY_INIT_CAPACITY * sizeof(Token));
    if (!array->tokens) {
        loginfo("Malloc Memory ERROR\n");
        return -1;
    }
    array->size = 0;
    array->capacity = TOKEN_ARRAY_INIT_CAPACITY;
    return 0;
}

void freeTokenArray(TokenArray *array) {
    free(array->tokens);
    array->tokens = NULL;
    array->size = 0;
    array->capacity = 0;
}

int addToken(TokenArray *array, const Token token) {
    // If array is full, expand by 2 times
    if (array->size == array->capacity) {
        array->capacity *= 2;
        Token *reallocated = realloc(array->tokens, array->capacity * sizeof(Token));
        if (reallocated == NULL) {
            loginfo("Allocating expended memory ERROR\n");
            return -1;
        }
        array->tokens = reallocated;
    }

    // Add new token
    array->tokens[array->size] = token;
    array->size++;
    return 0;
}

void deleteLastToken(TokenArray *array) {
    if (array->size == 0){
        loginfo("Deleting non existing element\n");
        return;
    }
    array->size--;
}


Token createToken(const TokenType type, const TokenAttribute attribute) {
    Token token;
    token.type = type;
    token.attribute = attribute; // TODO: INIT attribute

    return token;
}

int initStringAttribute(TokenAttribute *attr, const char *str) {
    attr->str = malloc(strlen(str) + 1);
    if (attr->str == NULL) {
        return -1;
    }
    strcpy(attr->str, str);
    return 0;
}

void freeToken(Token *token) {
    // free only if TokenType requires string attribute
    switch (token->type) {
        case TOKEN_ID:
        case TOKEN_STRING_LITERAL:
        case TOKEN_ERROR:
            if (token->attribute.str != NULL) {
                free(token->attribute.str);
                token->attribute.str = NULL;
            }
            break;
        default:
            // Otherwise do nothing
            break;
    }
}

const char *getTokenTypeName(const TokenType token) {
    switch (token) {
        case TOKEN_ID: return "TOKEN_ID";
        case TOKEN_KEYWORD_CONST: return "TOKEN_KEYWORD_CONST";
        case TOKEN_KEYWORD_VAR: return "TOKEN_KEYWORD_VAR";
        case TOKEN_KEYWORD_IF: return "TOKEN_KEYWORD_IF";
        case TOKEN_KEYWORD_ELSE: return "TOKEN_KEYWORD_ELSE";
        case TOKEN_KEYWORD_WHILE: return "TOKEN_KEYWORD_WHILE";
        case TOKEN_KEYWORD_FN: return "TOKEN_KEYWORD_FN";
        case TOKEN_KEYWORD_PUB: return "TOKEN_KEYWORD_PUB";
        case TOKEN_KEYWORD_NULL: return "TOKEN_KEYWORD_NULL";
        case TOKEN_KEYWORD_RETURN: return "TOKEN_KEYWORD_RETURN";
        case TOKEN_KEYWORD_VOID: return "TOKEN_KEYWORD_VOID";
        case TOKEN_KEYWORD_IMPORT: return "TOKEN_KEYWORD_IMPORT";
        case TOKEN_KEYWORD_I32: return "TOKEN_KEYWORD_I32";
        case TOKEN_KEYWORD_I32_NULLABLE: return "TOKEN_KEYWORD_I32_NULLABLE";
        case TOKEN_KEYWORD_F64: return "TOKEN_KEYWORD_F64";
        case TOKEN_KEYWORD_F64_NULLABLE: return "TOKEN_KEYWORD_F64_NULLABLE";
        case TOKEN_KEYWORD_U8: return "TOKEN_KEYWORD_U8";
        case TOKEN_KEYWORD_U8_ARRAY: return "TOKEN_KEYWORD_U8_ARRAY";
        case TOKEN_KEYWORD_U8_ARRAY_NULLABLE: return "TOKEN_KEYWORD_U8_ARRAY_NULLABLE";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";
        case TOKEN_I32_LITERAL: return "TOKEN_I32_LITERAL";
        case TOKEN_F64_LITERAL: return "TOKEN_F64_LITERAL";
        case TOKEN_ASSIGNMENT: return "TOKEN_ASSIGNMENT";
        case TOKEN_ADDITION: return "TOKEN_ADDITION";
        case TOKEN_SUBTRACTION: return "TOKEN_SUBTRACTION";
        case TOKEN_MULTIPLICATION: return "TOKEN_MULTIPLICATION";
        case TOKEN_DIVISION: return "TOKEN_DIVISION";
        case TOKEN_LESS_THAN: return "TOKEN_LESS_THAN";
        case TOKEN_LESS_THAN_OR_EQUAL_TO: return "TOKEN_LESS_THAN_OR_EQUAL_TO";
        case TOKEN_GREATER_THAN: return "TOKEN_GREATER_THAN";
        case TOKEN_GREATER_THAN_OR_EQUAL_TO: return "TOKEN_GREATER_THAN_OR_EQUAL_TO";
        case TOKEN_EQUAL_TO: return "TOKEN_EQUAL_TO";
        case TOKEN_NOT_EQUAL_TO: return "TOKEN_NOT_EQUAL_TO";
        case TOKEN_LEFT_ROUND_BRACKET: return "TOKEN_LEFT_ROUND_BRACKET";
        case TOKEN_RIGHT_ROUND_BRACKET: return "TOKEN_RIGHT_ROUND_BRACKET";
        case TOKEN_LEFT_SQUARE_BRACKET: return "TOKEN_LEFT_SQUARE_BRACKET";
        case TOKEN_RIGHT_SQUARE_BRACKET: return "TOKEN_RIGHT_SQUARE_BRACKET";
        case TOKEN_LEFT_CURLY_BRACKET: return "TOKEN_LEFT_CURLY_BRACKET";
        case TOKEN_RIGHT_CURLY_BRACKET: return "TOKEN_RIGHT_CURLY_BRACKET";
        case TOKEN_VERTICAL_BAR: return "TOKEN_VERTICAL_BAR";
        case TOKEN_QUESTION_MARK: return "TOKEN_QUESTION_MARK";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_DOT: return "TOKEN_DOT";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_ERROR: return "TOKEN_ERROR";
        default: return "UNKNOWN_TOKEN";
    }
}
