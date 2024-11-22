#include "logging.h"
#include "structs/bvs.h"

#include "target_gen/target_label_context.h"

/**********************************************************/
/* Private Functions */
/**********************************************************/

void constructLabel(char *name, int counter, char **label)
{
    size_t labelLength = strlen(name) + 1 + 4; // name + underscore + 4 digits

    *label = (char *)malloc(labelLength * sizeof(char));
    if (*label == NULL)
    {
        loginfo("Failed to allocate memory for label");
        exit(99);
    }

    sprintf(*label, "%s_%04d", name, counter);
}

/**********************************************************/
/* Public Functions Definitions */
/**********************************************************/

void TargetLC_Init(TargetLabelContext *ctx)
{
    BVS_Init(ctx->labels);
    ctx->counter = 0;
}

char *TargetLC_GetOrLoadFuncLabel(TargetLabelContext *ctx, char *name)
{
    char *label = BVS_Search(ctx->labels, name);

    if (label == NULL)
    {
        // Func label is not found. Construct it's name and insert it
        constructLabel(name, ctx->counter, &label);

        BVS_Insert(ctx->labels, name, label, strlen(label) + 1);
        ctx->counter++;
    }
    return label;
}

char *TargetLC_AddOtherLabel(TargetLabelContext *ctx, char *name)
{
    char *label;

    constructLabel(name, ctx->counter, &label);

    // Label is not inserted into the tree, because it's not a function label.
    // User does not need to retrieve it later in the code execution.
    ctx->counter++;

    return label;
}
