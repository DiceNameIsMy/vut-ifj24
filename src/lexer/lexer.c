#include "lexer/lexer.h"

#include <string.h>
#include <stdbool.h>

#include "logging.h"

#define KEYWORD_BUFFER_SIZE 256

// TODO: This struct supports only variables of length 256. If there is no limit for variable sizes, we'll need to change this implementation.
typedef struct {
    char keyword_buffer[KEYWORD_BUFFER_SIZE];
    int keyword_pointer;
} keyword_accumulator_t;

int add_letter_to_keyword(keyword_accumulator_t *k, const char c) {
    if (k->keyword_pointer >= KEYWORD_BUFFER_SIZE) {
        return -1;
    }
    k->keyword_buffer[k->keyword_pointer] = c;
    return 0;
}

lexer_t init_lexer(FILE *stream) {
    lexer_t lexer;
    lexer.source = stream;
    return lexer;
}

Token unexpected_input(const lexer_t *lexer, keyword_accumulator_t *keyword) {
    Token token;
    token.type = Invalid;

    loginfo("got invalid token with keyword buffer '%s'", keyword->keyword_buffer);

    return token;
}

Token get_next_token(const lexer_t *lexer) {
    keyword_accumulator_t keyword;
    keyword.keyword_pointer = 0;
    memset(keyword.keyword_buffer, 0, KEYWORD_BUFFER_SIZE);

    while (true) {
        // TODO: '"' character
        // TODO: A number character
        // TODO: ' ' and '\n' character (end keyword)
        const int c_int = fgetc(lexer->source);
        if (c_int == EOF) {
            const Token token = { .type = EndProgram };
            return token;
        }

        const char c = (char)c_int;
        switch (c) {
            default:
                if (!add_letter_to_keyword(&keyword, c)) {
                    return unexpected_input(lexer, &keyword);
                }
        }
    }

}

