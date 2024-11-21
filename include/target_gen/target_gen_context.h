#ifndef TARGET_GEN_CONTEXT_H
#define TARGET_GEN_CONTEXT_H

#include "structs/bvs.h"

typedef struct {
    // Search tree where 
    // - key is the label 
    // - value is the amount of its occurences
    BVS *scopes;
} TGContext;

void TGContext_Init(TGContext *context);
void TGContext_Destroy(TGContext *context);

int TGContext_AddScope(const char *scopeName);
int TGContext_RemoveScope(const char *scopeName);

// Labels in target code are named using the scope they are in.
// For example, in the main function, label for an early return would be "main_return".
char *TGContext_GetLabel(const char *labelName);
char *TGContext_GetVar(const char *labelName);

#endif // TARGET_GEN_CONTEXT_H