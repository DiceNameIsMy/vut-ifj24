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


TEST(nullable_array)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("id ?[]u8 id1;", &tokenArray);

    Token t;
    // id
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "id") != 0) {
        FAILCOMPS("Wrong attribute value", "id", t.attribute.str);
    }
    // ?[]u8
    if (!check_token(&t, TOKEN_KEYWORD_U8_ARRAY_NULLABLE)) {
        return;
    }
    // id1
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "id1") != 0) {
        FAILCOMPS("Wrong attribute value", "id1", t.attribute.str);
    }
ENDTEST

/* Must define miltiline
TEST(multiline_string)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("id \" in string \n \\\\ still inside\" \n dif_id", &tokenArray);

    Token t;
    // id
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "id") != 0) {
        FAILCOMPS("Wrong attribute value", "id", t.attribute.str);
    }
    // " in string \n \\ still inside"
    if (!check_token(&t, TOKEN_STRING_LITERAL)) {
        return;
    }
    if (strcmp(t.attribute.str, " in string \n \\ still inside") != 0) {
        FAILCOMPS("Wrong attribute value", " in string \n \\ still inside", t.attribute.str);
    }
    // dif_id
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "dif_id") != 0) {
        FAILCOMPS("Wrong attribute value", "dif_id", t.attribute.str);
    }
ENDTEST
*/

TEST(double_symbol_comparison)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("if (a >= b);", &tokenArray);

    Token t;
    // if
    if (!check_token(&t, TOKEN_KEYWORD_IF)) {
        return;
    }
    // (
    if (!check_token(&t, TOKEN_LEFT_ROUND_BRACKET)) {
        return;
    }
    // a
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "a") != 0) {
        FAILCOMPS("Wrong attribute value", "a", t.attribute.str);
    }
    // >=
    if (!check_token(&t, TOKEN_GREATER_THAN_OR_EQUAL_TO)) {
        return;
    }
    // b
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "b") != 0) {
        FAILCOMPS("Wrong attribute value", "b", t.attribute.str);
    }
    // )
    if (!check_token(&t, TOKEN_RIGHT_ROUND_BRACKET)) {
        return;
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }
ENDTEST

TEST(const_import)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("const ifj = @import(\"ifj24.zig\");", &tokenArray);

    Token t;
    // const
    if (!check_token(&t, TOKEN_KEYWORD_CONST)) {
        return;
    }
    // ifj
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "ifj") != 0) {
        FAILCOMPS("Wrong attribute value", "ifj", t.attribute.str);
    }
    // =
    if (!check_token(&t, TOKEN_ASSIGNMENT)) {
        return;
    }
    // @import
    if (!check_token(&t, TOKEN_KEYWORD_IMPORT)) {
        return;
    }
    // (
    if (!check_token(&t, TOKEN_LEFT_ROUND_BRACKET)) {
        return;
    }
    // "ifj24.zig"
    if (!check_token(&t, TOKEN_STRING_LITERAL)) {
        return;
    }
    if (strcmp(t.attribute.str, "ifj24.zig") != 0) {
        FAILCOMPS("Wrong attribute value", "ifj24.zig", t.attribute.str);
    }
    // )
    if (!check_token(&t, TOKEN_RIGHT_ROUND_BRACKET)) {
        return;
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }
ENDTEST

int main() {
    initTokenArray(&tokenArray);

    RUN_TESTS();

    freeTokenArray(&tokenArray);

    SUMMARIZE()
}
