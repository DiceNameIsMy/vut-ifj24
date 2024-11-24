//
// Created by nur on 16.11.24.
//

#ifndef TARGET_GEN_H
#define TARGET_GEN_H

#include <stdio.h>
#include "structs/ast.h"
#include "structs/symtable.h"

int generateTargetCode(ASTNode* root, FILE *output);

#endif //TARGET_GEN_H
