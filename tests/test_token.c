//
// Created by nur on 5.10.24.
//

#include <stdio.h>
#include <string.h>  // Added for strcmp

#include "token.h"
#include "test_utils.h"

TokenArray tokenArray;

TEST(add_i32_token)
    const TokenAttribute i32Attr = {.integer = 256};
    const Token i32Token = {.type = TOKEN_I32_LITERAL, i32Attr};
    addToken(&tokenArray, i32Token);

    if (tokenArray.size != 1) {
        FAIL("Expected 1 token, but got %zu", tokenArray.size);
    }
    if (tokenArray.tokens[0].type != TOKEN_I32_LITERAL) {
        FAIL("Expected token type TOKEN_I32_LITERAL, but got %d", tokenArray.tokens[0].type);
    }
    if (tokenArray.tokens[0].attribute.integer != i32Attr.integer) {
        FAIL("Expected token attribute %d, but got %d", i32Attr.integer, tokenArray.tokens[0].attribute.integer);
    }
ENDTEST

TEST(add_string_token)
    TokenAttribute strAttr;
    initStringAttribute(&strAttr, "Hello World!");
    const Token strToken = {.type = TOKEN_STRING_LITERAL, .attribute = strAttr};
    addToken(&tokenArray, strToken);

    if (tokenArray.size != 2) {
        FAIL("Expected 2 tokens, but got %zu", tokenArray.size);
    }
    if (tokenArray.tokens[1].type != TOKEN_STRING_LITERAL) {
        FAIL("Expected token type TOKEN_STRING_LITERAL, but got %d", tokenArray.tokens[1].type);
    }
    if (strcmp(tokenArray.tokens[1].attribute.str, "Hello World!") != 0) {
        FAIL("Expected string 'Hello World!', but got '%s'", tokenArray.tokens[1].attribute.str);
    }
ENDTEST

int main() {
    if (initTokenArray(&tokenArray) != 0) {
        FAIL("Failed to initialize token array");
        return -1;
    }

    RUN_TESTS();

    freeTokenArray(&tokenArray);

    SUMMARIZE()
}
