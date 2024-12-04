//
// Created by malbert on 10/5/24.
//
#include "ast.h"
#include "token.h"
#include "semantics.h"
#include <stdio.h>
#include <stdbool.h>

#ifndef VUT_IFJ_PARSER_H
#define VUT_IFJ_PARSER_H


type_t idType(Token token);
void AddFunctionsToSymTable(TokenArray *array, SymTable *table);
void match(TokenType expected);
void isMatch(TokenType expected);
ASTNode* parseInit(TokenArray* array, SymTable *table);
ASTNode* parseProgram();
ASTNode* parseProlog();
ASTNode* parseFunctionDefList();
ASTNode* parseFunctionDef();
ASTNode* parseParamList();
ASTNode* parseParamListTail();
ASTNode* parseReturnType();
ASTNode* parseType();
ASTNode* parseVarType();
ASTNode* parseStatementList();
ASTNode* parseStatement();
ASTNode* parseConstDeclaration();
ASTNode* parseExpression();
ASTNode* parseRelationalTail(ASTNode* left);
ASTNode* parseSimpleExpression();
ASTNode* parseTerm();
ASTNode* parseFactor();
ASTNode* parseFunctionCall();
ASTNode* parseVarDeclaration();
ASTNode* parseAssignmentOrFunctionCall();
ASTNode* parseIfCondition();
ASTNode* parseWhileCondition();
ASTNode* parseReturnStatement();
ASTNode* parseBlockStatement();
void if_malloc_error(const char* string);
Token get_next_token();


#endif //VUT_IFJ_PARSER_H
