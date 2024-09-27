//
// Created by nur on 27.9.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer/lexeme.h>
#include <stdarg.h>

void test_lexeme(const char *test_name, const char **input, const int count, ...) {

    FILE *fake_stdin = fmemopen((char *)*input, strlen(*input), "r");
    if (fake_stdin == NULL) {
        perror("Failed to create fake stdin");
        return;
    }

    va_list args;           // Declare a variable argument list
    va_start(args, count);  // Initialize args to store all values after count

    for (int i = 0; i < count; i++) {
        Lexeme lexeme;
        if (parse_lexeme(fake_stdin, &lexeme) == -1) {
            fprintf(stderr, "[Failure] %-20s: Failed to parse a lexeme. Input: [%s]\n", test_name, *input);
            return;
        }

        char *expected = va_arg(args, char*);

        if (strcmp(lexeme.value, expected) != 0) {
            fprintf(stderr, "[Failure] %-20s: Incorrect lexeme. Got: [%s] Expected: [%s]\n", test_name, lexeme.value, expected);
            free_lexeme(lexeme);
            return;
        }
    }

    fprintf(stderr, "\033[32m[Success] %-20s\033[0m\n", test_name);
}

int main() {
    const char *input = "i32 a = 15;";

    test_lexeme("Single input", &input, 1, "i32");
    test_lexeme("Parse Line", &input, 4, "i32", "a", "=", "15", ";");

    return 0;
}
