#ifndef TARGET_LABEL_CONTEXT_H
#define TARGET_LABEL_CONTEXT_H

#include "structs/bvs.h"

#include "target_gen/instructions.h"

/// @brief Used to generate unique names for identifiers.
///        Can be used for labels or variables within a frame.
typedef struct {
    // A tree where:
    // - original name is a key 
    // - generated unique name is a value
    BVS *identifiers;
    int counter;
} IdIndexer;

void IdIndexer_Init(IdIndexer *indexer);
void IdIndexer_Destroy(IdIndexer *indexer);

/// @brief Find or create a unique name.
bool IdIndexer_GetOrCreate(IdIndexer *indexer, char *name, char **out);

/// @brief Create a unique name for the given identifier. 
///        It is not stored, but the counter is incremented nonetheless.
char *IdIndexer_CreateOneTime(IdIndexer *indexer, char *name);

#endif // TARGET_LABEL_CONTEXT_H