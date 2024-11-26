#ifndef TARGET_FUNC_SCOPE_H
#define TARGET_FUNC_SCOPE_H

#include "structs/queue.h"

#include "target_gen/instructions.h"

typedef struct {
  Instruction *labelInstr;
  Queue *varDeclarationsQueue;
  Queue *otherInstructionsQueue;
} TargetFuncContext;

void TFC_Init(TargetFuncContext *scope);
void TFC_Destroy(TargetFuncContext *scope);

void TFC_SetFuncLabel(TargetFuncContext *scope, char *label);
void TFC_AddVar(TargetFuncContext *scope, Variable var);
void TFC_AddInst(TargetFuncContext *scope, Instruction inst);

bool TFC_IsEmpty(TargetFuncContext *scope);

/// @brief Returns the top instruction to print & removes it from the list.
/// @param scope 
/// @param inst 
Instruction TFC_PopNext(TargetFuncContext *scope);

#endif // TARGET_FUNC_SCOPE_H