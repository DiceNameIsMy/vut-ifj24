//
// Created by nur on 19.10.24.
//


#include <stdbool.h>
#include <stdlib.h>
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
        FAIL("Invalid token type. Expected %s, but got %s", getTokenTypeName(type), getTokenTypeName(t->type));
        return false;
    }
    return true;
}

bool check_tokens(const Token *expected, const int size) {
    int i = 0;
    while (i < size) {
        const Token expectedToken = expected[i];

        Token t;
        if (!check_token(&t, expectedToken.type)) {
            return false;
        }
        i++;
    }
    return true;
}

bool has_error_in_tokens() {
    const int idx_before = idx;
    idx = 0;
    Token t;
    while (try_get_token(&t)) {
        if (t.type == TOKEN_ERROR) {
            const Token lastToken = tokenArray.tokens[idx-2];
            FAIL("Encountered an error token at index %i. Token before is %s\n", idx, getTokenTypeName(lastToken.type));
            idx = idx_before;
            return true;
        }
    }
    idx = idx_before;
    return false;
}

int read_source_code(const char *filename, char **source_code) {
    if (source_code == NULL) {
        return -1;
    }

    FILE *source_code_stream = fopen(filename, "r");
    if (source_code_stream == NULL) {
        FAIL("Failed to open the source code file");
        return -1;
    }
    if (streamToString(source_code_stream, source_code) != 0) {
        FAIL("Failed to read the source code from the file");
        return -1;
    }
    return 0;
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
        FAIL("Invalid token type for keyword. Expected TOKEN_KEYWORD_I32");
    }

ENDTEST

TEST(i32_literal_starting_with_leading_zeros)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("02;", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_ERROR)) {
        return;
    }
ENDTEST

TEST(f64_literal_starting_with_leading_zeros)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("02.3;", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_ERROR)) {
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
        FAIL("Invalid token type for string literal. Expected TOKEN_STRING_LITERAL");
    }
ENDTEST

TEST(oneline_string_literal_not_terminated)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("\"abc\n", &tokenArray);

    Token t;
    if (!check_token(&t, TOKEN_ERROR)) {
        FAIL("Didn't get an error when fed with an unfinished string literal");
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

TEST(multiline_string)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("\\\\multiline 1\n\\\\multiline2\n     \\\\multiline3\n\t ;", &tokenArray);

    Token t;
    // id
    if (!check_token(&t, TOKEN_STRING_LITERAL)) {
        return;
    }
    if (strcmp(t.attribute.str, "multiline 1\nmultiline2\nmultiline3") != 0) {
        FAILCOMPS("Wrong attribute value", "multiline 1\nmultiline2\nmultiline3", t.attribute.str);
    }
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    }
ENDTEST

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

TEST (dot_id_error)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("a.b;", &tokenArray);

    Token t;
    // a.b
    if (!check_token(&t, TOKEN_ERROR)) {
        return;
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    } 
ENDTEST

TEST (dot_id_space_separator)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("a . b;", &tokenArray);

    Token t;
    // a .b
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "a") != 0) {
        FAILCOMPS("Wrong attribute value", "a", t.attribute.str);
    }
    // .
    if (!check_token(&t, TOKEN_DOT)) {
        return;
    }
    // b
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "b") != 0) {
        FAILCOMPS("Wrong attribute value", "b", t.attribute.str);
    }
    // ;
    if (!check_token(&t, TOKEN_SEMICOLON)) {
        return;
    } 
ENDTEST

TEST (ifj_dot)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("ifj.write now", &tokenArray);

    Token t;
    // ifj.write
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "ifj.write") != 0) {
        FAILCOMPS("Wrong attribute value", "ifj.write", t.attribute.str);
    }
    // now
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "now") != 0) {
        FAILCOMPS("Wrong attribute value", "now", t.attribute.str);
    }
ENDTEST

TEST (ifj_tab_next_line)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("ifj\n.\twrite now", &tokenArray);

    Token t;
    // ifj.write
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "ifj.write") != 0) {
        FAILCOMPS("Wrong attribute value", "ifj.write", t.attribute.str);
    }
    // now
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "now") != 0) {
        FAILCOMPS("Wrong attribute value", "now", t.attribute.str);
    }
ENDTEST

TEST (ifj_part_of_id)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    runLexer("ifjword u8 now", &tokenArray);
    Token t;
    // ifjword
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "ifjword") != 0) {
        FAILCOMPS("Wrong attribute value", "ifjword", t.attribute.str);
    }
    // u8
    if (!check_token(&t, TOKEN_KEYWORD_U8)) {
        return;
    }
    // now
    if (!check_token(&t, TOKEN_ID)) {
        return;
    }
    if (strcmp(t.attribute.str, "now") != 0) {
        FAILCOMPS("Wrong attribute value", "now", t.attribute.str);
    }    
ENDTEST


TEST(parse_basic_program)
    char *source_code;
    if (read_source_code("tests/input/program.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;

    runLexer(source_code, &tokenArray);
    free(source_code);

    const Token expected[] = {
        {.type = TOKEN_KEYWORD_CONST},
        {.type = TOKEN_ID, .attribute.str = "ifj"},
    };
    check_tokens(expected, 2);

    if (has_error_in_tokens()) {
        return;
    }
ENDTEST

TEST(parse_iterative_factorial_program)
    char *source_code;
    if (read_source_code("tests/input/factorial_iter.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;

    runLexer(source_code, &tokenArray);
    free(source_code);

    if (has_error_in_tokens()) {
        return;
    }
ENDTEST

TEST(parse_recursive_factorial_program)
    char *source_code;
    if (read_source_code("tests/input/factorial_rec.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;

    runLexer(source_code, &tokenArray);
    free(source_code);

    if (has_error_in_tokens()) {
        return;
    }
ENDTEST

TEST(parse_stdlib_funcs_program)
    char *source_code;
    if (read_source_code("tests/input/stdlib_funcs.ifj24.zig", &source_code)) {
        return;
    }
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;

    runLexer(source_code, &tokenArray);
    free(source_code);

    if (has_error_in_tokens()) {
        return;
    }

ENDTEST

TEST(example_from_task)
    freeTokenArray(&tokenArray);
    initTokenArray(&tokenArray);
    idx = 0;
    // "Ahoj\n\"Sve'te \"
    runLexer("\"Ahoj\\n\\\"Sve'te \\\x22\"", &tokenArray);
    Token t;
    // Ahoj\n
    // "Sve'te \"
    if (!check_token(&t, TOKEN_STRING_LITERAL)) {
        return;
    }
    if (strcmp(t.attribute.str, "Ahoj\n\"Sve'te \"") != 0) {
        FAILCOMPS("Wrong attribute value", "Ahoj\nSve'te \"", t.attribute.str);
    }
ENDTEST


int main() {
    initTokenArray(&tokenArray);

    RUN_TESTS();

    freeTokenArray(&tokenArray);

    SUMMARIZE()
}
