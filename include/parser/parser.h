//
// Created by malbert on 10/5/24.
//

#ifndef VUT_IFJ_PARSER_H
#define VUT_IFJ_PARSER_H

#include "structs/ast.h"
#include "lexer/token.h"

void match(TokenType expected);
ASTNode* parseInit(TokenArray* array);
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
ASTNode* parseIfStatement();
ASTNode* parseWhileStatement();
ASTNode* parseReturnStatement();
Token get_next_token();
Token get_previous_token();

#endif //VUT_IFJ_PARSER_H
