#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "bvs.h"

#include "id_indexer.h"

/**********************************************************/
/* Private Functions */
/**********************************************************/

void constructUniqueName(char *name, int counter, char **label)
{
    size_t labelLength = strlen(name) + 1 + 4 + 1; // name + underscore + 4 digits + \0 byte

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

void IdIndexer_Init(IdIndexer *indexer)
{
    indexer->identifiers = malloc(sizeof(BVS));
    if (indexer->identifiers == NULL)
    {
        loginfo("Failed to allocate memory for BVS");
        exit(99);
    }
    BVS_Init(indexer->identifiers);
    indexer->counter = 0;
}

void IdIndexer_Destroy(IdIndexer *indexer)
{
    BVS_Free(indexer->identifiers);
    free(indexer->identifiers);
    indexer->counter = 0;
}

bool IdIndexer_GetOrCreate(IdIndexer *indexer, char *name, char **out)
{
    bool created = false;
    char *label = BVS_Search(indexer->identifiers, name);
    if (label == NULL)
    {
        // Func label is not found. Construct it's name and insert it
        label = IdIndexer_CreateOneTime(indexer, name);
        BVS_Insert(indexer->identifiers, name, label, strlen(label) + 1);
        created = true;
    }
    *out = label;

    return created;
}

char *IdIndexer_CreateOneTime(IdIndexer *indexer, char *name)
{
    char *label;
    constructUniqueName(name, indexer->counter, &label);

    indexer->counter++;
    return label;
}
