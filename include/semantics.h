#include "symtable.h"
#include "token.h"
#include "ast.h"
#include "types.h"

#ifndef VUT_SEMANTICS_H
#define VUT_SEMANTICS_H

type_t Sem_MathConv(ASTNode *left, ASTNode *right, ASTNode *higher_order);
type_t Sem_AssignConv(ASTNode *left, ASTNode *right, ASTNode *higher_order);
bool isRound(double literal);
void PerformArithm(ASTNode *left, ASTNode *right, ASTNode *higher_order);


#endif
