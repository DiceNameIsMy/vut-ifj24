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

void BVSBranch_Init(BVSBranch *newbranch, const long data, BVS_Color color) { //maybe we should put a pointer as an argument? Maybe we should make this a void-function?
    newbranch = malloc(sizeof(BVSBranch));
    if (newbranch == NULL) {
        return NULL;
    }
    newbranch->color = color;
    newbranch->data = data;
    newbranch->left = NULL;
    newbranch->right = NULL;
    newbranch->parent = NULL;
    //return newbranch;
}

void BVSBranch_Free(BVSBranch *branch) {
    if (branch == NULL) {
        return;
    }
    BVSBranch_Free(branch->left);
    BVSBranch_Free(branch->right);
    free(branch);
}

void BVSBranch_Insert(BVSBranch *branch, const long data) {
    //TODO: WHY THE FUCK DOES AN INSERT PROCEDURE RETURNS A VALUE? ELIMINATE THIS IMMEDIATELY!
    if (branch == NULL) {
        BVSBranch_Init(branch, data, RED);
        BVSBranch_InsertResolve(branch);
        return;
    }
    
    if (branch->data > data) {
        BVSBranch_Insert(branch->left, data);
        branch->left->parent = branch;
    } else if (branch->data < data) { //should we consider the case that branch->data == data?
        BVSBranch_Insert(branch->right, data);
        branch->right->parent = branch;
    }
    return;
    //return branch; //at the end it returns the branch you modified, at the top level that's the ROOT. TF honestly 
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
