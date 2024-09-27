//
// Created by nur on 27.9.24.
//

#include <stdio.h>
#include <string.h>
#include <lexer/lexeme.h>
#include <stdarg.h>
#include <stdbool.h>

LexemeParser *init_test_parser(const char **input) {
    FILE *fake_stdin = fmemopen((char *)*input, strlen(*input), "r");
    if (fake_stdin == NULL) {
        perror("Failed to create fake stdin");
        return NULL;
    }

    return init_lexeme_parser(fake_stdin);
}

void test_lexeme(const char *test_name, const char **input, const int count, ...) {
    LexemeParser *parser = init_test_parser(input);
    if (parser == NULL) {
        fprintf(stderr, "[Failure] %-20s: Failed to init a lexeme parser.\n", test_name);
        return;
    }

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        Lexeme lexeme;
        if (next_lexeme(parser, &lexeme) == -1) {
            fprintf(stderr, "[Failure] %-20s: Failed to parse a lexeme. Input: [%s]\n", test_name, *input);
            destroy_lexeme_parser(parser);
            return;
        }
        char *expected = va_arg(args, char*);

        const bool match = strcmp(lexeme.value, expected) == 0;
        destroy_lexeme(lexeme);

        if (!match) {
            fprintf(stderr, "[Failure] %-20s: Incorrect lexeme. Got: [%s] Expected: [%s]\n", test_name, lexeme.value, expected);
            destroy_lexeme_parser(parser);
            return;
        }
    }

    fprintf(stderr, "\033[32m[Success] %-20s\033[0m\n", test_name);
    destroy_lexeme_parser(parser);
}

int main() {
    const char *input = "i32 a = 15;\nint main() {}\n";

    test_lexeme("Single input", &input, 1, "i32");
    test_lexeme("Line", &input, 5, "i32", "a", "=", "15", ";");
    test_lexeme("Multi Line", &input, 11, "i32", "a", "=", "15", ";", "int", "main", "(", ")", "{", "}");

    return 0;
}
