#include <stdlib.h>

#include "logging.h"

#include "structs/queue.h"

#include "target_gen/target_func_scope.h"

void TargetFS_Init(TargetFuncScope *scope) {
    if (scope == NULL) {
        loginfo("Function scope is NULL when trying to initialize it.");
        exit(99);
    }
    scope->varDeclarationsQueue = malloc(sizeof(Queue));
    if (scope->varDeclarationsQueue == NULL) {
        loginfo("Failed to allocate memory for the variable declarations queue.");
        exit(99);
    }
    scope->otherInstructionsQueue = malloc(sizeof(Queue));
    if (scope->otherInstructionsQueue == NULL) {
        loginfo("Failed to allocate memory for the other instructions queue.");
        exit(99);
    }

    Queue_Init(scope->varDeclarationsQueue);
    Queue_Init(scope->otherInstructionsQueue);
}

void TargetFS_Destroy(TargetFuncScope *scope) {
    if (!TargetFS_IsEmpty(scope)) {
        loginfo("Function scope is not empty when trying to destroy it.");
        exit(99);
    }
    if (Queue_Destroy(scope->varDeclarationsQueue) != 0) {
        loginfo("Failed to destroy the variable declarations queue.");
        exit(99);
    }    
    if (Queue_Destroy(scope->otherInstructionsQueue) != 0) {
        loginfo("Failed to destroy the other instructions queue.");
        exit(99);
    }

    free(scope->varDeclarationsQueue);
    free(scope->otherInstructionsQueue);
}

void TargetFS_AddVar(TargetFuncScope *scope, Variable var) {
    Queue_Enqueue(scope->varDeclarationsQueue, &var);
}
void TargetFS_AddInst(TargetFuncScope *scope, Instruction inst) {
    Queue_Enqueue(scope->otherInstructionsQueue, &inst);
}

bool TargetFS_IsEmpty(TargetFuncScope *scope) {
    return Queue_IsEmpty(scope->varDeclarationsQueue) && Queue_IsEmpty(scope->otherInstructionsQueue);
}

Instruction TargetFS_PopNext(TargetFuncScope *scope) {
    // First, all variables must be defined
    if (!Queue_IsEmpty(scope->varDeclarationsQueue)) {

        Variable *var = NULL;
        Queue_Dequeue(scope->varDeclarationsQueue, (void **)&var);
        if (var == NULL) {
            loginfo("No more variables to pop from the function scope, even though there should be");
            exit(99);
        }

        Instruction inst = initInstr1(INST_DEFVAR, initVarOperand(OP_VAR, var->frame, var->name));

        free(var);

        return inst;
    }

    // Then, other instructions can be printed
    if (!Queue_IsEmpty(scope->otherInstructionsQueue)) {

        Instruction *instPtr = NULL;
        Queue_Dequeue(scope->otherInstructionsQueue, (void **)&instPtr);
        if (instPtr == NULL) {
            loginfo("No more instructions to pop from the function scope, even though there should be");
            exit(99);
        }

        Instruction inst = *instPtr;
        free(instPtr);
        return inst;
    }

    // There aren't any instructions left to pop. Invalid function call.
    loginfo("No more instructions to pop from the function scope.");
    exit(99);
}
