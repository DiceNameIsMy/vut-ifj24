#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token.h"

int streamToString(FILE *stream, char **str);

void runLexer(const char* sourceCode, TokenArray *tokenArray);

#endif // LEXER_H
