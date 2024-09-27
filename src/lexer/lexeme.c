//
// Created by nur on 27.9.24.
//
#include "lexer/lexeme.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"

#define MAX_LEXEME_LENGTH 256
#define LEXEME_PARSING_STRING "%256s"

int parse_lexeme(FILE *stream, Lexeme *lexeme) {
    char value[MAX_LEXEME_LENGTH];

    if (fscanf(stream, LEXEME_PARSING_STRING, value) != 1) {
        free(lexeme->value);
        loginfo("failed to read a lexeme from a stream");
        return -1;
    }

    const unsigned long length = strlen(value);
    if (length == MAX_LEXEME_LENGTH) {
        loginfo("read lexeme [%32s] is too long (%i max)", value, MAX_LEXEME_LENGTH);
        return -1;
    }

    lexeme->value = malloc((length + 1) * sizeof(char));
    if (lexeme->value == NULL) {
        loginfo("failed to allocate space for a lexeme");
        return -1;
    }

    strcpy(lexeme->value, value);

    return 0;
}

void free_lexeme(const Lexeme lexeme) {
    if (lexeme.value != NULL) {
        free(lexeme.value);
    }
}
