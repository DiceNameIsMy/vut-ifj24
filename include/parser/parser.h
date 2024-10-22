//
// Created by malbert on 10/5/24.
//

#ifndef VUT_IFJ_PARSER_H
#define VUT_IFJ_PARSER_H
#include "lexer/token.h"
#include "lexer/lexer.h"

void parseInit(TokenArray* array);
void parseProgram();
void match(TokenType expected);
void parseProlog();
void parseFunctionDefList();
void parseFunctionDef();
void parseParamList();
void parseParamListTail();
void parseReturnType();
void parseType();
void parseVarType();
void parseStatementList();
void parseStatement();
void parseConstDeclaration();
void parseExpression();
void parseRelationalTail();
void parseSimpleExpression();
void parseTerm();
void parseFactor();
void parseFunctionCall();
void parseVarDeclaration();
void parseAssignmentOrFunctionCall();
void parseIfStatement();
void parseWhileStatement();
void parseReturnStatement();
Token get_next_token();

#endif //VUT_IFJ_PARSER_H
