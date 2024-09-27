//
// Created by nur on 27.9.24.
//

#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>

typedef struct Lexeme {
    char *value;
} Lexeme;

int parse_lexeme(FILE *stream, Lexeme *lexeme);
void free_lexeme(Lexeme lexeme);

#endif //LEXEME_H
