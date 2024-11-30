#include <stdlib.h>

#include "logging.h"

#include "queue.h"

#include "target_func_context.h"

void TFC_Init(TargetFuncContext *scope)
{
    if (scope == NULL)
    {
        loginfo("Function scope is NULL when trying to initialize it.");
        exit(99);
    }
    scope->varDeclarationsQueue = malloc(sizeof(Queue));
    if (scope->varDeclarationsQueue == NULL)
    {
        loginfo("Failed to allocate memory for the variable declarations queue.");
        exit(99);
    }
    scope->otherInstructionsQueue = malloc(sizeof(Queue));
    if (scope->otherInstructionsQueue == NULL)
    {
        loginfo("Failed to allocate memory for the other instructions queue.");
        exit(99);
    }

    scope->labelInstr = NULL;
    Queue_Init(scope->varDeclarationsQueue);
    Queue_Init(scope->otherInstructionsQueue);
}

void TFC_Destroy(TargetFuncContext *scope)
{
    if (!TFC_IsEmpty(scope))
    {
        loginfo("Function scope is not empty when trying to destroy it.");
        exit(99);
    }
    if (Queue_Destroy(scope->varDeclarationsQueue) != 0)
    {
        loginfo("Failed to destroy the variable declarations queue.");
        exit(99);
    }
    if (Queue_Destroy(scope->otherInstructionsQueue) != 0)
    {
        loginfo("Failed to destroy the other instructions queue.");
        exit(99);
    }
    if (scope->labelInstr != NULL)
    {
        free(scope->labelInstr);
    }
    free(scope->varDeclarationsQueue);
    free(scope->otherInstructionsQueue);
}

void TFC_SetFuncLabel(TargetFuncContext *scope, char *label)
{
    if (scope->labelInstr != NULL)
    {
        loginfo("Function scope already has a label instruction.");
        exit(99);
    }

    scope->labelInstr = malloc(sizeof(Instruction));
    if (scope->labelInstr == NULL)
    {
        loginfo("Failed to allocate memory for the label instruction.");
        exit(99);
    }
    *scope->labelInstr = initInstr1(INST_LABEL, initStringOperand(OP_LABEL, label));
}

void TFC_AddVar(TargetFuncContext *scope, Variable var)
{
    Instruction *instPtr = malloc(sizeof(Instruction));
    if (instPtr == NULL)
    {
        loginfo("Failed to allocate memory for the instruction.");
        exit(99);
    }
    *instPtr = initInstr1(INST_DEFVAR, initVarOperand(OP_VAR, var.frame, var.name));

    Queue_Enqueue(scope->varDeclarationsQueue, instPtr);
}
void TFC_AddInst(TargetFuncContext *scope, Instruction inst)
{
    Instruction *instPtr = malloc(sizeof(Instruction));
    if (instPtr == NULL)
    {
        loginfo("Failed to allocate memory for the instruction.");
        exit(99);
    }
    *instPtr = inst;

    Queue_Enqueue(scope->otherInstructionsQueue, instPtr);
}

bool TFC_IsEmpty(TargetFuncContext *scope)
{
    return Queue_IsEmpty(scope->varDeclarationsQueue) && Queue_IsEmpty(scope->otherInstructionsQueue);
}

Instruction TFC_PopNext(TargetFuncContext *scope)
{
    Instruction *instPtr = NULL;

    // First, process the label, then DEFVARs, then function body
    if (scope->labelInstr != NULL)
    {
        instPtr = scope->labelInstr;
        scope->labelInstr = NULL;
    }
    else if (!Queue_IsEmpty(scope->varDeclarationsQueue))
    {
        Queue_Dequeue(scope->varDeclarationsQueue, (void **)&instPtr);
        if (instPtr == NULL)
        {
            loginfo("No more variables to pop from the function scope, even though there should be");
            exit(99);
        }
    }
    else if (!Queue_IsEmpty(scope->otherInstructionsQueue))
    {
        Queue_Dequeue(scope->otherInstructionsQueue, (void **)&instPtr);
        if (instPtr == NULL)
        {
            loginfo("No more instructions to pop from the function scope, even though there should be");
            exit(99);
        }
    }
    else
    {
        // There aren't any instructions left to pop. Invalid function call.
        loginfo("No more instructions to pop from the function scope.");
        exit(99);
    }

    Instruction inst = *instPtr;
    free(instPtr);
    return inst;
}
