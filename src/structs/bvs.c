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

BVSBranch *BVSBranch_Init(long data, BVS_Color color);
void BVSBranch_Free(BVSBranch *branch);
BVSBranch *BVSBranch_Insert(BVSBranch *branch, long data);
bool BVSBranch_Search(BVSBranch *branch, long data);
bool BVSBranch_Delete(BVSBranch *branch, long data);

// Returns a new subRoot that now must stand in place of *branch
BVSBranch *BVSBranch_LeftRotate(BVSBranch *branch);
// Returns a new subRoot that now must stand in place of *branch
BVSBranch *BVSBranch_RightRotate(BVSBranch *branch);

// Internal definitions

BVSBranch *BVSBranch_Init(const long data, BVS_Color color) {
    BVSBranch *branch = malloc(sizeof(BVSBranch));
    if (branch == NULL) {
        return NULL;
    }
    branch->color = color;
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
        return BVSBranch_Init(data, RED);
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

BVSBranch *BVSBranch_LeftRotate(BVSBranch *branch) {
    BVSBranch *newRoot = branch->right;
    branch->right = branch->right->left;
    newRoot->left = branch;
    return newRoot;
}

BVSBranch *BVSBranch_RightRotate(BVSBranch *branch) {
    BVSBranch *newRoot = branch->left;
    branch->left = branch->left->right;
    newRoot->right = branch;
    return newRoot;
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
    if (bvs->root == NULL) {
        bvs->root = BVSBranch_Init(data, BLACK);
    } else {
        bvs->root = BVSBranch_Insert(bvs->root, data);
    }
}

bool BVS_Search(BVS *bvs, const long data) {
    return BVSBranch_Search(bvs->root, data);
}
