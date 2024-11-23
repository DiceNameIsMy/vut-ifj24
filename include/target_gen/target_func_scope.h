#ifndef TARGET_FUNC_SCOPE_H
#define TARGET_FUNC_SCOPE_H

#include "structs/queue.h"

#include "target_gen/instructions.h"

typedef struct {
  Queue *varDeclarationsQueue;
  Queue *otherInstructionsQueue;
} TargetFuncScope;

void TargetFS_Init(TargetFuncScope *scope);
void TargetFS_Destroy(TargetFuncScope *scope);

void TargetFS_AddVar(TargetFuncScope *scope, Variable var);
void TargetFS_AddInst(TargetFuncScope *scope, Instruction inst);

bool TargetFS_IsEmpty(TargetFuncScope *scope);

/// @brief Returns the top instruction to print & removes it from the list.
/// @param scope 
/// @param inst 
Instruction TargetFS_PopNext(TargetFuncScope *scope);

#endif // TARGET_FUNC_SCOPE_H