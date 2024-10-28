#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "lexer/token.h"

#define TOKEN_ARRAY_INIT_CAPACITY 16

int initTokenArray(TokenArray *array) {
    array->tokens = (Token *) malloc(TOKEN_ARRAY_INIT_CAPACITY * sizeof(Token));
    if (!array->tokens) {
        fprintf(stderr, "Malloc Memory ERROR\n");
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
            fprintf(stderr, "Allocating expended memory ERROR\n");
            return -1;
        }
        array->tokens = reallocated;
    }

    // Add new token
    array->tokens[array->size] = token;
    array->size++;
    return 0;
}

int deleteLastToken(TokenArray *array){
    if (array->size == 0){
        fprintf(stderr, "Deleting non existing element\n");
        return -1;
    }
    array->size--;
}


Token createToken(const TokenType type, const TokenAttribute attribute) {
    Token token;
    token.type = type;
    token.attribute = attribute; // TODO: INIT attribute

    return token;
}

int initStringAttribute(TokenAttribute *attr, char *str) {
    attr->str = malloc(strlen(str) + 1);
    if (attr->str == NULL) {
        return -1;
    }
    strcpy(attr->str, str);
    return 0;
}

void freeToken(Token *token) {  // TODO: Potentially broken due to token->attribute.str
    if (token->attribute.str) {
        free(token->attribute.str);
        token->attribute.str = NULL;
    }
}
