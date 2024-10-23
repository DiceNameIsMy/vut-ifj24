//
// Created by nur on 19.10.24.
//
#include "structs/bvs.h"

#include <stdlib.h>
#include<stdio.h>

// Internal declarations

typedef enum {
    RED,
    BLACK
} BVS_Color;

void BVSBranch_Init(BVSBranch **newbranch, long data, BVS_Color color);
void BVSBranch_Free(BVSBranch *branch);
void BVSBranch_Insert(BVSBranch *branch, long data);
bool BVSBranch_Search(BVSBranch *branch, long key);
void BVSBranch_Delete(BVSBranch *branch, long key); //deletion and search is by the key, data just looks strange
bool BVSBranch_IsRoot(BVSBranch *branch);

// Rotates around the *branch
void BVSBranch_LeftRotate(BVSBranch *branch);
void BVSBranch_RightRotate(BVSBranch *branch);
// Resolves the tree structure after an insert operation
void BVSBranch_InsertResolve(BVSBranch *branch);
//resolves the tree structure after a delete operation
void BVSBranch_DeleteResolve(BVSBranch *branch);
void Help_RmDoubleBlack(BVSBranch *branch); //removes doubleblack property from the node

// Internal definitions

void BVSBranch_Init(BVSBranch **newbranch, const long data, BVS_Color color) { //maybe we should put a pointer as an argument? Maybe we should make this a void-function?
    *newbranch = (BVSBranch *)malloc(sizeof(BVSBranch));
    if (*newbranch == NULL) {
        return;
    }
    (*newbranch)->color = color;
    (*newbranch)->data = data;
    (*newbranch)->left = NULL;
    (*newbranch)->right = NULL;
    (*newbranch)->parent = NULL;
    return;
    //return newbranch;
}

void BVSBranch_Free(BVSBranch *branch) {
    //TODO: maybe we can un-recur this 
    if (branch == NULL) {
        return;
    }
    BVSBranch_Free(branch->left);
    BVSBranch_Free(branch->right);
    free(branch);
    return;
}

void BVSBranch_InsertResolve(BVSBranch *branch) {
    //TODO: perhaps we even can un-recur this...
    if (BVSBranch_IsRoot(branch)) { //if root, just PAINT IT BLACK(c)
        branch->color = BLACK;
        return;
    }
    if (branch->parent->color == BLACK) //also covers the case when father is root
    {
        fprintf(stderr, "Father is black, returning...\n");
        return;
    }

    fprintf(stderr, "Not root, the father is red\n");
    BVSBranch *father = branch->parent;
    BVSBranch *granpa = father->parent; 
    fprintf(stderr, "Father ang granpa exist\n");
    BVSBranch *uncle = (granpa->left == father) ? granpa->right : granpa->left;
    if (uncle != NULL && uncle->color == RED) { //The "Red Uncle" case
        father->color = BLACK;
        uncle->color = BLACK;
        if (!BVSBranch_IsRoot(granpa)) { //if not root, paint the grandfather red
            granpa->color = RED;
            BVSBranch_InsertResolve(granpa);
        } else {
            granpa->color = BLACK;
        }
        return; //quit to un-nest the ifs
    }
    //black uncle case
    if (branch == father->left && father == granpa->left) { //LL-scenario
        BVSBranch_RightRotate(granpa);
    } else if (branch == father->right && father == granpa->left) { //LR-scenario
        BVSBranch_LeftRotate(father);
        BVSBranch_RightRotate(granpa);
    } else if (branch == father->left && father == granpa->right) {//RL-scenario
        BVSBranch_RightRotate(father);
        BVSBranch_LeftRotate(granpa);
    } else if (branch == father->right && father == granpa->right) {//RR-scenario
        BVSBranch_LeftRotate(granpa);
    }
    char buffer = granpa->color; //colour swap common for all the rest of the cases
    granpa->color = father->color;
    father->color = buffer; 
    return;
}

void BVSBranch_Insert(BVSBranch *branch, const long data) {
    BVSBranch *current = NULL;
    BVSBranch *next = branch;
    

    while (true) {
        current = next;
        if (data < current->data) {
            next = next->left;
            if (next == NULL) {
                fprintf(stderr, "Initializing %ld on the left of %ld\n", data, current->data);
                BVSBranch_Init(&(current->left), data, RED);
                current->left->parent = current;
                BVSBranch_InsertResolve(current->left);
                break;
            }
        } else if (data > current->data) {
            next = next->right;
            if (next == NULL) {
                fprintf(stderr, "Initializing %ld on the right of %ld\n", data, current->data);
                BVSBranch_Init(&(current->right), data, RED);
                current->right->parent = current;
                BVSBranch_InsertResolve(current->right);
                break;
            }
        } else break;
    }
    return;
}

bool BVSBranch_Search(BVSBranch *branch, long key) {
    //TODO: un-recur this
    if (branch == NULL)
        return false;

    if (branch->data == key) {
        return true;
    }

    // Search in branches
    if (branch->data > key) {
        return BVSBranch_Search(branch->left, key);
    }
    return BVSBranch_Search(branch->right, key);
}

void Help_RmDoubleBlack(BVSBranch *branch) {
    //TODO: and yes, we can also un-recur this
    if (BVSBranch_IsRoot(branch)) {
        branch->color = BLACK;
        return;
    }
    BVSBranch *sibling = (branch->parent->left == branch) ? branch->parent->right : branch->parent->left;
    //red sibling scenario
    if (sibling->color == RED) { //sibling is never NULL
        branch->parent->color = RED; //recolor nodes
        sibling->color = BLACK;
        if (branch->parent->left == sibling) { //and rotate
            //TODO: check if the rotations are done as expected
            BVSBranch_RightRotate(branch->parent); //Left scenario
            sibling = branch->parent->right;
        } else {
            BVSBranch_LeftRotate(branch->parent); //Right scenario
            sibling = branch->parent->left;
        }
    } //by now the red sibling scenario is transformed either to second or the third one
    //black sibling with red nephews scenario
    BVSBranch *red_nephew = NULL; //check for a red nephew
    if (sibling->left != NULL && sibling->left->color == RED) {
        red_nephew = sibling->left;
    } else if (sibling->right != NULL && sibling->right->color == RED) {
        red_nephew = sibling->right;
    }
    if (red_nephew != NULL) { //there is one or two
        if (sibling->parent->left == sibling && sibling->left == red_nephew) { //LL-scenario
            //TODO
            BVSBranch_RightRotate(branch->parent);
            red_nephew->color = BLACK;
            return;
        } //THERE'S NO ELSE...
        if (sibling->parent->left == sibling && sibling->right == red_nephew) { //LR-scenario
            //TODO
            red_nephew->color = BLACK;
            sibling->color = RED;
            BVSBranch_LeftRotate(sibling);
            BVSBranch_RightRotate(branch->parent);
            return;
        }
        if (sibling->parent->right == sibling && sibling->left == red_nephew) { //RL-scenario
            //TODO
            red_nephew->color = BLACK;
            sibling->color = RED;
            BVSBranch_RightRotate(sibling);
            BVSBranch_LeftRotate(branch->parent);
            return;
        }
        if (sibling->parent->right == sibling && sibling->right == red_nephew) { //RR-scenario
            //TODO
            BVSBranch_LeftRotate(branch->parent);
            red_nephew->color = BLACK;
            return;
        }
    }
    //black sibling with black nephews scenario
    sibling->color = RED;
    if (branch->parent->color == RED) {
        branch->parent->color = BLACK;
        return;
    }
    Help_RmDoubleBlack(branch->parent);
    return;
}

void BVSBranch_DeleteResolve(BVSBranch *branch) {
    //TODO: implement for one descendant and no descendants
    if (branch->color == RED) { //if red node removed, nothing to do
        return;
    }
    BVSBranch *red_son = NULL;
    if (branch->left != NULL && branch->left->color == RED) {
        red_son = branch->left;
    } else if (branch->right != NULL && branch->right->color == RED) {
        red_son = branch->right;
    }
    if (red_son == NULL) { //for no descendants
        Help_RmDoubleBlack(branch); //in certain scenario, recur for its parent
        return;          
    }
    red_son->color = BLACK; //for one (obviously RED) descendant
    return;
}

void BVSBranch_Delete(BVSBranch *branch, long key) {
    if (branch == NULL)
        return;
    if (branch->data > key) { //recur until we reach the key or NULL
        BVSBranch_Delete(branch->left, key);
        return;
    } else if (branch->data < key){
        BVSBranch_Delete(branch->right, key);
        return;
    }
    if (branch->left == NULL && branch->right == NULL) { //just delete the node if it has no successors
        BVSBranch *tmp_father = branch->parent;
        //BVSBranch *tmp = branch;
        if (tmp_father->left == branch) {
            tmp_father->left = NULL;
        } else {
            tmp_father->right = NULL;
        }
        BVSBranch_DeleteResolve(branch); //to resolve the deletion we should act as if the node is still there
        free(branch);
        return;
    }
    if (branch->left == NULL || branch->right == NULL) { //exactly one successor
        BVSBranch *not_null = (branch->left == NULL) ? branch->right : branch->left;
        not_null->parent = branch->parent;
        if (branch->parent->left == branch) {
            branch->parent->left = not_null;
        } else {
            branch->parent->right = not_null;
        }
        BVSBranch_DeleteResolve(branch);
        free(branch);
        return;
    }
    //if there are two successors
    BVSBranch *leftmost = branch->right; //basically, we delete this node and copy its data to branch
    while (leftmost->left != NULL) {
        leftmost = leftmost->left;
    }
    branch->data = leftmost->data; //we replace node's data with the data of its in-order successor
    leftmost->parent->left = leftmost->right; //we delete the leftmost node
    if (leftmost->right != NULL) { //if any right successor
        leftmost->right->parent = leftmost->parent;
    }
    BVSBranch_DeleteResolve(leftmost);
    free(leftmost);
    // TODO: Implement the resolving function for deletion (I'll do it)
    return;
}

void BVSBranch_LeftRotate(BVSBranch *branch) {
    //CHECK THIS PLZ!!!
    fprintf(stderr, "LEFTROTATE (%ld)!\n", branch->data);
    branch->right->parent = branch->parent; //right must exist...
    if (!BVSBranch_IsRoot(branch)) {
        if (branch->parent->left == branch) {
            branch->parent->left = branch->right;
        } else {
            branch->parent->right = branch->right;
        }
    }
    branch->parent = branch->right; //right must exist =>parent must exist
    branch->right = branch->right->left; //now it might be NULL
    branch->parent->left = branch; //parent is always non-zero
    if (branch->right != NULL) { //might be NULL, trust me
        branch->right->parent = branch;
    }
    return;
}

void BVSBranch_RightRotate(BVSBranch *branch) {
    //CHECK THIS PLZ!!!
    fprintf(stderr, "RIGHTROTATE (%ld)!\n", branch->data);
    branch->left->parent = branch->parent;
    if (!BVSBranch_IsRoot(branch)) { 
        if (branch->parent->left == branch) {
            branch->parent->left = branch->left;
        } else {
            branch->parent->right = branch->left;
        }
    }
    branch->parent = branch->left;
    branch->left = branch->left->right; //we presume that branch->left != NULL
    branch->parent->right = branch;
    if (branch->left != NULL) {
        branch->left->parent = branch;
    }
    return;
}

bool BVSBranch_IsRoot(BVSBranch *branch) {
    return branch->parent == NULL;
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
        BVSBranch_Init(&(bvs->root), data, BLACK);
    } else {
        BVSBranch_Insert(bvs->root, data);
    }
}

bool BVS_Search(BVS *bvs, const long data) {
    return BVSBranch_Search(bvs->root, data);
}
