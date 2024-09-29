//
// Created by nur on 27.9.24.
//
#include "lexer/lexeme.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"

#define BUFFER_LENGTH 512
#define LOAD_BUFFER_STRING "%511s"
#define MAX_LEXEME_LENGTH 256
#define LEXEME_PARSING_STRING "%256s"
// TODO: Expand the characters that end a lexeme
const char *LEXEME_SKIP_CHARS = " \t\n\r";
const char *LEXEME_STOP_CHARS = ".,;(){} \t\n\r";

/*
 * Load part of the source code to the buffer.
 * Use it when you've processed everything from the buffer.
 */
int load_source_code_buffer(LexemeParser *parser) {
    const size_t bytes_read = fread(parser->buffer, 1, BUFFER_LENGTH-1, parser->stream);
    if (bytes_read != BUFFER_LENGTH-1 && !feof(parser->stream)) {
        loginfo("failed to read from a stream");
        return -1;
    }
    parser->cursor = 0;
    return 0;
}

int init_lexeme_parser(LexemeParser *parser, FILE *stream) {
    if (stream == NULL) {
        return -1;
    }

    parser->cursor = 0;
    parser->stream = stream;
    parser->buffer = malloc(BUFFER_LENGTH * sizeof(char));
    if (parser->buffer == NULL) {
        return -1;
    }

    if (load_source_code_buffer(parser) == -1) {
        loginfo("failed to read from a stream");
        free(parser->buffer);
        return -1;
    }


    return 0;
}

void destroy_lexeme_parser(const LexemeParser *parser) {
    if (parser == NULL) {
        return;
    }
    if (parser->buffer != NULL) {
        free(parser->buffer);
    }
}

char get_char(LexemeParser *parser) {
    if (parser->cursor == BUFFER_LENGTH) {
        load_source_code_buffer(parser);
    }
    return parser->buffer[parser->cursor];
}

int next_lexeme(LexemeParser *parser, Lexeme *lexeme) {
    if (parser == NULL) {
        return -1;
    }
    if (lexeme == NULL) {
        return -1;
    }

    // Load a lexeme
    int cursor = 0;
    char value[MAX_LEXEME_LENGTH] = {'\0'};

    char c;
    while (true) {
        c = get_char(parser);
        if (c == EOF || c == '\0') {
            break;
        }

        const bool is_stop_char = strchr(LEXEME_STOP_CHARS, c) != NULL;
        const bool end_lexeme = is_stop_char && cursor != 0;
        if (end_lexeme) {
            loginfo("ending a lexeme [%32s] as a stop symbol %c was encountered", value, c);
            break;
        }

        // Do not include whitespaces in a lexeme
        const bool is_whitespace = strchr(LEXEME_SKIP_CHARS, c) != NULL;
        if (is_whitespace) {
            parser->cursor++;
            continue;
        }

        // Store the char & continue loading the lexeme
        if (cursor == MAX_LEXEME_LENGTH) {
            loginfo("read lexeme [%32s] is too long (%i max)", value, MAX_LEXEME_LENGTH);
            return -1;
        }
        value[cursor] = c;
        parser->cursor++;
        cursor++;
    }

    // Store the loaded lexeme
    lexeme->value = malloc((cursor + 1) * sizeof(char));
    if (lexeme->value == NULL) {
        loginfo("failed to allocate space for a lexeme [%s]", value);
        return -1;
    }
    strcpy(lexeme->value, value);
    return 0;
}

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

void destroy_lexeme(const Lexeme lexeme) {
    if (lexeme.value != NULL) {
        free(lexeme.value);
    }
}
