//
// Created by nur on 19.10.24.
//
#include "structs/bvs.h"

#include <stdlib.h>

// Internal declarations

typedef enum {
    RED,
    BLACK
} BVS_Color;

BVSBranch *BVSBranch_Init(long data);
void BVSBranch_Free(BVSBranch *branch);
BVSBranch *BVSBranch_Insert(BVSBranch *branch, long data);
bool BVSBranch_Search(BVSBranch *branch, long data);
bool BVSBranch_Delete(BVSBranch *branch, long data);

// Internal definitions

BVSBranch *BVSBranch_Init(const long data) {
    BVSBranch *branch = malloc(sizeof(BVSBranch));
    if (branch == NULL) {
        return NULL;
    }
    branch->color = BLACK;
    branch->data = data;
    branch->left = NULL;
    branch->right = NULL;
    return branch;
}

void BVSBranch_Free(BVSBranch *branch) {
    if (branch == NULL) {
        return;
    }
    BVSBranch_Free(branch->left);
    BVSBranch_Free(branch->right);
    free(branch);
}

BVSBranch *BVSBranch_Insert(BVSBranch *branch, const long data) {
    if (branch == NULL) {
        return BVSBranch_Init(data);
    }

    if (branch->data > data) {
        branch->left = BVSBranch_Insert(branch->left, data);
    } else if (branch->data < data) {
        branch->right = BVSBranch_Insert(branch->right, data);
    }
    return branch;
}

bool BVSBranch_Search(BVSBranch *branch, long data) {
    if (branch == NULL)
        return false;

    if (branch->data == data) {
        return true;
    }

    // Search in branches
    if (branch->data > data) {
        return BVSBranch_Search(branch->left, data);
    }
    return BVSBranch_Search(branch->right, data);
}

bool BVSBranch_Delete(BVSBranch *branch, long data) {
    if (branch == NULL)
        return false;

    // TODO: Implement
    return false;
}

// Header file definitions

int BVS_Init(BVS *bvs) {
    bvs->root = NULL;
    return 0;
}


void BVS_Free(const BVS *bvs) {
    if (bvs->root == NULL) {
        return;
    }
    BVSBranch_Free(bvs->root);
}

void BVS_Insert(BVS *bvs, const long data) {
    bvs->root = BVSBranch_Insert(bvs->root, data);
}

bool BVS_Search(BVS *bvs, const long data) {
    return BVSBranch_Search(bvs->root, data);
}
