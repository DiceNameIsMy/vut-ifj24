//
// Created by nur on 19.10.24.
//

#ifndef BVS_H
#define BVS_H

#include <stdbool.h>

typedef struct BVSBranch {
    long data;
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
void BVS_Free(const BVS *bvs);
void BVS_Delete(BVS *BVS, const long key);
void BVS_Insert(BVS *bvs, long data);
BVSBranch *BVS_Search(BVS *bvs, long data);
//bool BVS_IsBalanced(BVS *bvs);

#endif //BVS_H
