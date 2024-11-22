#ifndef TARGET_LABEL_CONTEXT_H
#define TARGET_LABEL_CONTEXT_H

#include "structs/bvs.h"

#include "target_gen/instructions.h"

typedef struct {
    // A tree where:
    // - original name is a key 
    // - target label is a value
    BVS *labels;
    int counter;
} TargetLabelContext;

void TargetLC_Init(TargetLabelContext *ctx);

/// @brief Find a label representing function name. If doesn't exist, create one.
char *TargetLC_GetOrLoadFuncLabel(TargetLabelContext *ctx, char *name);

/// @brief Add a label. Used to make jumps for conditinals or loops. 
///        Once you got a label, you must use it and free it. 
///        Subsequent calls with the same name will return a new label.
char *TargetLC_AddOtherLabel(TargetLabelContext *ctx, char *name);

#endif // TARGET_LABEL_CONTEXT_H