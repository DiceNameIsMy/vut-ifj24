//
// Created by malbert on 10/5/24.
//

#ifndef VUT_IFJ_PARSER_H
#define VUT_IFJ_PARSER_H
#include "lexer/token.h"
#include "lexer/lexer.h"

void parser(lexer_t *lexer);
void parseProgram(lexer_t *lexer);
void match(lexer_t *lexer, token_type_t expected);
void parseProlog(lexer_t *lexer);
void parseFunctionDefList(lexer_t *lexer);
void parseFunctionDef(lexer_t *lexer);
void parseParamList(lexer_t *lexer);
void parseParamListTail(lexer_t *lexer);
void parseReturnType(lexer_t *lexer);
void parseType(lexer_t *lexer);
void parseStatement(lexer_t *lexer);

#endif //VUT_IFJ_PARSER_H
