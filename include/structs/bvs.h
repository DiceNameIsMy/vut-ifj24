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
} BVSBranch;

// Vyskove Vyvazeni Binarni Vyhledavaci Strom
typedef struct {
    BVSBranch *root;
} BVS;

int BVS_Init(BVS *bvs);
void BVS_Free(const BVS *bvs);

void BVS_Insert(BVS *bvs, long data);
bool BVS_Search(BVS *bvs, long data);

#endif //BVS_H
