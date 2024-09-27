#include <stdio.h>
#include <stdbool.h>

#include "logging.h"
#include "lexer/lexer.h"

int main(void) {
    loginfo("Hello, World!%i\n", 10);
    printf("Hello, World!\n");

    lexer_t lexer = init_lexer(stdin);

    while (true) {
        const Token t = get_next_token(&lexer);
        if (t.type == EndProgram) {
            loginfo("Program has ended");
            return 0;
        }
        if (t.type == Invalid) {
            loginfo("Got invalid token");
            return -1;
        }

        loginfo("Token type: %i, processing of which is not yet implemented", t.type);
        return -1;
    }
}
