//
// Created by nur on 19.10.24.
//

/* Before you add anything to the BST make sure that:
*  - you have free the "key" later if you malloc-ed it
*  - same with "data"
*/

#ifndef BVS_H
#define BVS_H

#include <stdbool.h>
#include <stddef.h>

typedef struct BVSBranch {
    char *key; //differentiate key and data
    void *data;
    char color;
    struct BVSBranch *left;
    struct BVSBranch *right;
    struct BVSBranch *parent; //wer gonna need this for colour resolving routines a lot
} BVSBranch;

// Vyskove Vyvazeni Binarni Vyhledavaci Strom
typedef struct {
    BVSBranch *root;
} BVS;

int BVS_Init(BVS *bvs);
void BVS_Free(BVS *bvs);
void BVS_Delete(BVS *BVS, char *key);
void BVS_Insert(BVS *bvs, char *key, void *data, size_t size); //size field is a crutch so that we can work with a copy of the data
BVSBranch *BVS_Search(BVS *bvs, char *key);

#endif //BVS_H
