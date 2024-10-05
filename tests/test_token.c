//
// Created by nur on 5.10.24.
//

#include <stdio.h>
#include <string.h>  // Added for strcmp

#include "lexer/token.h"

int main() {
    const char *test_name = "add 2 tokens";

    TokenArray tokenArray;
    if (initTokenArray(&tokenArray) != 0) {
        fprintf(stderr, "[Error] %-20s: Failed to initialize token array\n", test_name);
        return -1;
    }

    const token_attribute i32Attr = {.integer = 256};
    const token_t i32Token = {.type = TOKEN_I32_LITERAL, i32Attr};
    addToken(&tokenArray, i32Token);

    if (tokenArray.size != 1) {
        fprintf(stderr, "[Failure] %-20s: Expected 1 token, but got %zu\n", test_name, tokenArray.size);
    }
    if (tokenArray.tokens[0].type != TOKEN_I32_LITERAL) {
        fprintf(stderr, "[Failure] %-20s: Expected token type TOKEN_I32_LITERAL, but got %d\n", test_name, tokenArray.tokens[0].type);
    }
    if (tokenArray.tokens[0].attribute.integer != i32Attr.integer) {
        fprintf(stderr, "[Failure] %-20s: Expected token attribute %d, but got %d\n", test_name, i32Attr.integer, tokenArray.tokens[0].attribute.integer);
    }

    token_attribute strAttr;
    initStringAttribute(&strAttr, "Hello World!");
    const token_t strToken = {.type = TOKEN_STRING_LITERAL, .attribute = strAttr};
    addToken(&tokenArray, strToken);

    if (tokenArray.size != 2) {
        fprintf(stderr, "[Failure] %-20s: Expected 2 tokens, but got %zu\n", test_name, tokenArray.size);
    }
    if (tokenArray.tokens[1].type != TOKEN_STRING_LITERAL) {
        fprintf(stderr, "[Failure] %-20s: Expected token type TOKEN_STRING_LITERAL, but got %d\n", test_name, tokenArray.tokens[1].type);
    }
    if (strcmp(tokenArray.tokens[1].attribute.str, "Hello World!") != 0) {
        fprintf(stderr, "[Failure] %-20s: Expected string 'Hello World!', but got '%s'\n", test_name, tokenArray.tokens[1].attribute.str);
    }

    freeTokenArray(&tokenArray);

    return 0;
}
