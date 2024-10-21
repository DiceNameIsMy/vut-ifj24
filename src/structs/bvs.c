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

void BVSBranch_Init(BVSBranch *newbranch, long data, BVS_Color color);
void BVSBranch_Free(BVSBranch *branch);
void BVSBranch_Insert(BVSBranch *branch, long data);
bool BVSBranch_Search(BVSBranch *branch, long data);
bool BVSBranch_Delete(BVSBranch *branch, long data);

// Rotates around the *branch
void BVSBranch_LeftRotate(BVSBranch *branch);
void BVSBranch_RightRotate(BVSBranch *branch);
// Resolves the tree structure after an insert operation
void BVSBranch_InsertResolve(BVSBranch *branch)

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

void BVSBranch_InsertResolve(BVSBranch *branch) {
    if (BVSBranch_IsRoot(branch)) { //if root, just PAINT IT BLACK(c)
        branch->color = BLACK;
        return;
    }
    if (branch->parent->color == BLACK) //also covers the case when father is root
        return;

    BVSBranch *father = branch->parent;
    BVSBranch *granpa = father->parent; 
    if (granpa->left->color == RED && granpa->right->color == RED) { //The "Red Uncle" case
        granpa->right->color = BLACK;
        granpa->left->color = BLACK;
        if (!BVSBranch_IsRoot(granpa)) { //if not root, paint the grandfather red
            granpa->color = RED;
            BVSBranch_InsertResolve(granpa);
        } else {
            granpa->color = BLACK;
        }
        return; //quit to un-nest the ifs
    }
    if (branch == father->left && father == granpa->left) { //LL-scenario
        BVSBranch_RightRotate(granpa);
        char buffer = granpa->color; //possibly could be outside the ifs
        granpa->color = father->color; //possibly could be outside the ifs
        father->color = buffer; //possibly could be out outside the ifs
    } else if (branch == father->right && father == granpa->left) { //LR-scenario
        BVSBranch_LeftRotate(father);
        BVSBranch_RightRotate(granpa);
        char buffer = granpa->color;
        granpa->color = father->color;
        father->color = buffer;
    } else if (branch == father->left && father == granpa->right) {//RL-scenario
        BVSBranch_RightRotate(father);
        BVSBranch_LeftRotate(granpa);
        char buffer = granpa->color;
        granpa->color = father->color;
        father->color = buffer;
    } else if (branch == father->right && father == granpa->right) {//RR-scenario
        BVSBranch_LeftRotate(granpa);
        char buffer = granpa->color;
        granpa->color = father->color;
        father->color = buffer;
    }
    return;
}

void BVSBranch_Insert(BVSBranch *branch, const long data) {
    if (branch == NULL) {
        BVSBranch_Init(branch, data, RED);//TODO: cover the case when branch = NULL
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

void BVSBranch_LeftRotate(BVSBranch *branch) {
    BVSBranch *newRoot = branch->right; //propagate an error if branch->right == NULL
    branch->right = branch->right->left;
    newRoot->left = branch;
}

void BVSBranch_RightRotate(BVSBranch *branch) {
    BVSBranch *newRoot = branch->left;
    branch->left = branch->left->right; //what if branch->left = NULL
    newRoot->right = branch;
}

bool BVSBranch_IsRoot(BVSBranch *branch) {
    return branch == NULL;
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
