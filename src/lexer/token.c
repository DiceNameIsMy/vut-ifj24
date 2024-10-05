#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "lexer/token.h"

#define TOKEN_ARRAY_INIT_CAPACITY 16

int initTokenArray(TokenArray* array) {
    array->tokens = (token_t*) malloc(TOKEN_ARRAY_INIT_CAPACITY * sizeof(token_t));
    if (!array->tokens) {
        fprintf(stderr, "Malloc Memory ERROR\n");
        return -1;
    }
    array->size = 0;
    array->capacity = TOKEN_ARRAY_INIT_CAPACITY;
    return 0;
}

void freeTokenArray(TokenArray* array) {
    free(array->tokens);
    array->tokens = NULL;
    array->size = 0;
    array->capacity = 0;
}

int addToken(TokenArray* array, const token_t token) {
    // If array is full, expand by 2 times
    if (array->size == array->capacity) {
        array->capacity *= 2;
        token_t* reallocated = realloc(array->tokens, array->capacity * sizeof(token_t));
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


token_t createToken(const token_type_t type, const token_attribute attribute) {
    token_t token;
    token.type = type;
    token.attribute = attribute;

    return token;
}
int initStringAttribute(token_attribute* attr, char* str) {
    attr->str = malloc(strlen(str) + 1);
    if (attr->str == NULL) {
        return -1;
    }
    strcpy(attr->str, str);
    return 0;
}

void freeToken(token_t* token) {
    if (token->attribute.str != NULL) {
        free(token->attribute.str);
        token->attribute.str = NULL;
    }
}