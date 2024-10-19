//
// Created by nur on 19.10.24.
//


#include <stdbool.h>
#include <tgmath.h>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "test_utils.h"

TokenArray tokenArray;
static int idx = 0;

// returns true if token exists
bool try_get_token(Token *t) {
    const bool has_token = tokenArray.size > idx;
    if (has_token) {
        *t = tokenArray.tokens[idx++];
        return true;
    }

    return false;
}

// return true if token is valid
bool check_token(Token *t, const TokenType type) {
    if (!try_get_token(t)) {
        FAIL("Failed to load the next token. TokenArray size: %i, idx of a token: %i", (int)tokenArray.size, idx);
        return false;
    }
    if (t->type != type) {
        FAIL("Invalid token type. Expected %i, but got %i", type, t->type);
        return false;
    }
    return true;
}

TEST(empty)
    // Reinit token array on each test run
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("", &tokenArray);

    Token t;
    if (try_get_token(&t)) {
        FAIL("There are tokens with empty source code provided");
    }
ENDTEST

TEST(keyword)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("i32", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_KEYWORD_I32)) {
        return;
    }

ENDTEST

TEST(string_literal)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("\"abc\"", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_STRING_LITERAL)) {
        return;
    }
ENDTEST

TEST(oneline_string_literal_not_terminated)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("\"abc\n", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_ERROR)) {
        return;
    }
ENDTEST

TEST(oneline_string_literal_not_terminated__source_code_ended)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("\"abc", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_ERROR)) {
        return;
    }
ENDTEST

TEST(float_literals)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("1.2; .2; 1.2E3; .2e-3", &tokenArray);

    Token t;

    // 1.2
    if (!check_token(&t, TOKEN_F64_LITERAL)) {
        return;
    }
    if (fabs(t.attribute.real - 1.2) > 0.0000001) {
        FAILCOMPF("Invalid float literal (1.2)", 1.2, t.attribute.real);
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }

    // .2
    if (!check_token(&t, TOKEN_F64_LITERAL)) {
        return;
    }
    if (fabs(t.attribute.real - .2) > 0.0000001) {
        FAILCOMPF("Invalid float literal (.2)", .2, t.attribute.real);
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }

    // 1.2E3
    if (!check_token(&t, TOKEN_F64_LITERAL)) {
        return;
    }
    if (fabs(t.attribute.real - 1.2E3) > 0.0000001) {
        FAILCOMPF("Invalid float literal (1.2E3)", 1.2E3, t.attribute.real);
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }

    // .2e-3
    if (!check_token(&t, TOKEN_F64_LITERAL)) {
        return;
    }
    if (fabs(t.attribute.real - .2e-3) > 0.0000001) {
        FAILCOMPF("Invalid float literal (.2e-3)", .2e-3, t.attribute.real);
    }

    if (try_get_token(&t)) {
        FAIL("Got more tokens than expected. There are: %i, expected: %i", (int)tokenArray.size, idx);
    }

ENDTEST

TEST(assignment)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("i32 a = b + 3;", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_KEYWORD_I32)) {
        return;
    }
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "a") != 0) {
        FAILCOMPS("Wrong attribute value", "a", t.attribute.str);
    }
    if (!check_token(&t, TOKEN_ASSIGNMENT)) {
        return;
    }
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "b") != 0) {
        FAILCOMPS("Wrong attribute value", "b", t.attribute.str);
    }
    if (!check_token(&t, TOKEN_ADDITION)) {
        return;
    }
    if (!check_token(&t, TOKEN_I32_LITERAL)) {
        return;
    }
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }

ENDTEST

int main() {
    initTokenArray(&tokenArray);

    RUN_TESTS();

    return 0;
}
