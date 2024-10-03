//
// Created by nur on 27.9.24.
//

#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>

// A struct used to generate the next lexeme on the fly.
typedef struct {
    FILE *stream;
    char *buffer;
    int cursor;
} LexemeParser;

typedef struct {
    char *value;
} Lexeme;

int LexemeParser_Init(LexemeParser *parser, FILE *stream);
void LexemeParser_Destroy(LexemeParser *parser);

// Get a next lexeme from the lexeme parser.
// Don't forget to free up space after getting each lexeme!!!
int LexemeParser_GetNext(LexemeParser *parser, Lexeme *lexeme);

void destroy_lexeme(Lexeme lexeme);

#endif //LEXEME_H
