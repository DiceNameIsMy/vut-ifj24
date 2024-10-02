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
int LexemeParser_LoadBuffer(LexemeParser *parser) {
    const size_t bytes_read = fread(parser->buffer, 1, BUFFER_LENGTH-1, parser->stream);
    if (bytes_read != BUFFER_LENGTH-1 && !feof(parser->stream)) {
        loginfo("failed to read from a stream");
        return -1;
    }
    parser->cursor = 0;
    return 0;
}

int LexemeParser_Init(LexemeParser *parser, FILE *stream) {
    if (stream == NULL) {
        return -1;
    }

    parser->cursor = 0;
    parser->stream = stream;
    parser->buffer = malloc(BUFFER_LENGTH * sizeof(char));
    if (parser->buffer == NULL) {
        return -1;
    }

    if (LexemeParser_LoadBuffer(parser) == -1) {
        loginfo("failed to read from a stream");
        free(parser->buffer);
        return -1;
    }

    return 0;
}

void LexemeParser_Destroy(LexemeParser *parser) {
    if (parser == NULL) {
        return;
    }
    if (parser->buffer != NULL) {
        free(parser->buffer);
    }
    parser->cursor = 0;
}

char LexemeParser_GetChar(LexemeParser *parser) {
    if (parser->cursor == BUFFER_LENGTH) {
        LexemeParser_LoadBuffer(parser);
    }
    return parser->buffer[parser->cursor];
}

void LexemeParser_MarkCharProcessed(LexemeParser *parser) {
    parser->cursor++;
}

int LexemeParser_GetNext(LexemeParser *parser, Lexeme *lexeme) {
    if (parser == NULL) {
        return -1;
    }
    if (lexeme == NULL) {
        return -1;
    }

    int next_char_idx = 0;
    char lexeme_value[MAX_LEXEME_LENGTH] = {'\0'};

    while (true) {
        const char c = LexemeParser_GetChar(parser);
        if (c == EOF || c == '\0') {
            break;
        }

        const bool is_stop_char = strchr(LEXEME_STOP_CHARS, c) != NULL;
        const bool lexeme_value_is_empty = next_char_idx == 0;
        const bool end_lexeme = is_stop_char && !lexeme_value_is_empty;
        if (end_lexeme) {
            loginfo("ending a lexeme [%32s] as a stop symbol %c was encountered", value, c);
            break;
        }

        // Ignore whitespaces in a lexeme
        const bool is_whitespace = strchr(LEXEME_SKIP_CHARS, c) != NULL;
        if (is_whitespace) {
            LexemeParser_MarkCharProcessed(parser);
            continue;
        }

        if (next_char_idx == MAX_LEXEME_LENGTH) {
            loginfo("read lexeme [%32s] is too long (%i max)", value, MAX_LEXEME_LENGTH);
            return -1;
        }
        // Set next lexeme letter
        lexeme_value[next_char_idx] = c;
        next_char_idx++;

        LexemeParser_MarkCharProcessed(parser);
    }

    // Store the loaded lexeme
    lexeme->value = malloc((next_char_idx + 1) * sizeof(char));
    if (lexeme->value == NULL) {
        loginfo("failed to allocate space for a lexeme [%s]", value);
        return -1;
    }
    strcpy(lexeme->value, lexeme_value);
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
