//
// Created by nur on 19.10.24.
// Implemented by oleh 26.10.24
//
#include "structs/bvs.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
//#include <stdio.h> for logs

// Internal declarations

typedef enum {
    RED,
    BLACK
} BVS_Color;

void BVSBranch_Init(BVSBranch **newbranch, char *key, void *data, size_t size); //size field is a crutch
void BVSBranch_Free(BVSBranch *branch);
void *BVSBranch_Search(BVSBranch *branch, char *key);

BVSBranch *BVSBranch_Insert(BVSBranch *branch, char *key, void *data, size_t size);
void BVSBranch_InsertResolve(BVSBranch *branch); // Resolves the tree structure after an insert operation

BVSBranch *BVSBranch_Delete(BVSBranch *branch, char *key); //deletion and search is by the key, data just looks strange
void BVSBranch_DeleteResolve(BVSBranch *branch); //resolves the tree structure after a delete operation
void Help_RmDoubleBlack(BVSBranch *branch); //removes doubleblack property from the node

// Rotates around the *branch
void BVSBranch_LeftRotate(BVSBranch *branch);
void BVSBranch_RightRotate(BVSBranch *branch);

int BVSBranch_Height(BVSBranch *branch);
BVSBranch *BVSBranch_Root(BVSBranch *BVSBranch); //finds a corresponding root
bool BVSBranch_IsRoot(BVSBranch *branch); //predicate if the nose is root

// Internal definitions

void BVSBranch_Init(BVSBranch **newbranch, char *key, void *data, size_t size) { 
    *newbranch = (BVSBranch *)malloc(sizeof(BVSBranch));
    if (*newbranch == NULL) {
        return;
    }
    (*newbranch)->color = RED;
    (*newbranch)->data = (void *)malloc(size);
    memcpy((*newbranch)->data, data, size); //data should be malloc-ed, NEVER allocated on local scope!
    (*newbranch)->key = strdup(key);
    (*newbranch)->left = NULL;
    (*newbranch)->right = NULL;
    (*newbranch)->parent = NULL;
    return;
}

void BVSBranch_Free(BVSBranch *branch) {
    if (branch == NULL) {
        return;
    }
    BVSBranch_Free(branch->left);
    BVSBranch_Free(branch->right);
    free(branch->key);
    free(branch->data);
    free(branch);
    return;
}

void BVSBranch_InsertResolve(BVSBranch *branch) {
    if (BVSBranch_IsRoot(branch)) { //if root, just PAINT IT BLACK(c)
        branch->color = BLACK;
        return;
    }
    if (branch->parent->color == BLACK) //also covers the case when father is root
    {
        //fprintf(stderr, "Father is black, returning...\n");
        return;
    }

    //fprintf(stderr, "Not root, the father is red\n");
    BVSBranch *father = branch->parent;
    BVSBranch *granpa = father->parent; 
    //fprintf(stderr, "Father ang granpa exist\n");
    BVSBranch *uncle = (granpa->left == father) ? granpa->right : granpa->left;
    if (uncle != NULL && uncle->color == RED) { //The "Red Uncle" case
        //fprintf(stderr, "Uncle is red, recurring...\n");
        father->color = BLACK;
        uncle->color = BLACK;
        granpa->color = RED;
        BVSBranch_InsertResolve(granpa);
        return; //quit to un-nest the ifs
    }
    //black uncle case
    if (branch == father->left && father == granpa->left) { //LL-scenario
        BVSBranch_RightRotate(granpa);
        granpa->color = RED;
        father->color = BLACK; 
    } else if (branch == father->right && father == granpa->left) { //LR-scenario
        BVSBranch_LeftRotate(father);
        BVSBranch_RightRotate(granpa);
        granpa->color = RED;
        branch->color = BLACK; 
    } else if (branch == father->left && father == granpa->right) {//RL-scenario
        BVSBranch_RightRotate(father);
        BVSBranch_LeftRotate(granpa);
        granpa->color = RED;
        branch->color = BLACK; 
    } else if (branch == father->right && father == granpa->right) {//RR-scenario
        BVSBranch_LeftRotate(granpa);
        granpa->color = RED;
        father->color = BLACK; 
    }
    return;
}

BVSBranch *BVSBranch_Insert(BVSBranch *branch, char *key, void *data, size_t size) {
    BVSBranch *current = branch;
    BVSBranch *next = branch;

    while (next != NULL && strcmp(next->key, key) != 0) {
        current = next;
        if (strcmp(key, current->key) < 0) {
            next = current->left;
        } else if (strcmp(key, current->key) > 0) {
            next = current->right;
        } else break;
    }

    if (next == NULL) {
        if (strcmp(key, current->key) > 0) {
            BVSBranch_Init(&(current->right), key, data, size);
            current->right->parent = current;
            BVSBranch_InsertResolve(current->right);
        } else if (strcmp(key, current->key) < 0) {
            BVSBranch_Init(&(current->left), key, data, size);
            current->left->parent = current;
            BVSBranch_InsertResolve(current->left);
        }
    }
    return BVSBranch_Root(current);
}

void *BVSBranch_Search(BVSBranch *branch, char *key) {

    BVSBranch *current = branch;
    while (current != NULL && strcmp(current->key, key) != 0) {
        if (strcmp(key, current->key) > 0) {
            current = current->right;
        } else {
            current = current->left;
        }
    }

    if (current != NULL)
        return current->data;
    return NULL;
}

void Help_RmDoubleBlack(BVSBranch *branch) {
    if (BVSBranch_IsRoot(branch)) {
        //fprintf(stderr, "Ended up in root, stop recurring\n");
        branch->color = BLACK;
        return;
    }
    BVSBranch *sibling = (branch->parent->left == branch || branch->parent->left == branch->left || branch->parent->left == branch->right) ? branch->parent->right : branch->parent->left; //remember we nullified the father's right/left?
    //red sibling scenario
    //fprintf(stderr, "Calculated the former sibling\n");
    if (sibling->color == RED) { //sibling is never NULL
        //fprintf(stderr, "Sibling was red\n");
        branch->parent->color = RED; //recolor nodes
        sibling->color = BLACK;
        if (branch->parent->left == sibling) { //and rotate
            //fprintf(stderr, "Left scenario\n");
            sibling = sibling->right;
            BVSBranch_RightRotate(branch->parent); //Left scenario
        } else {
            //fprintf(stderr, "Right scenario\n");
            sibling = sibling->left;
            BVSBranch_LeftRotate(branch->parent); //Right scenario
        }
    } //by now the red sibling scenario is transformed either to second or the third one
    //black sibling with red nephews scenario
    //fprintf(stderr, "By now sibling (new one) is black\n");
    BVSBranch *red_nephew = NULL; //check for a red nephew
    int red_nephew_cnt = 0;
    if (sibling->left != NULL && sibling->left->color == RED) {
        red_nephew = sibling->left;
        red_nephew_cnt++;
    } 
    if (sibling->right != NULL && sibling->right->color == RED) {
        red_nephew = sibling->right;
        red_nephew_cnt++;
    }
    if (red_nephew != NULL) { //there is one or two
        //fprintf(stderr, "there is a red nephew\n");
        if (sibling->parent->left == sibling && (sibling->left == red_nephew || red_nephew_cnt == 2)) { //LL-scenario
            sibling->color = branch->parent->color;
            branch->parent->color = BLACK;
            sibling->left->color = BLACK;
            BVSBranch_RightRotate(branch->parent);
            return;
        } //THERE'S NO ELSE...
        if (sibling->parent->right == sibling && (sibling->right == red_nephew || red_nephew_cnt == 2)) { //RR-scenario
            sibling->color = branch->parent->color;
            branch->parent->color = BLACK;
            sibling->right->color = BLACK;
            BVSBranch_LeftRotate(branch->parent);
            return;
        }
        if (sibling->parent->left == sibling && sibling->right == red_nephew) { //LR-scenario
            sibling->color = BLACK;
            red_nephew->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_LeftRotate(sibling);
            BVSBranch_RightRotate(branch->parent);
            return;
        }
        if (sibling->parent->right == sibling && sibling->left == red_nephew) { //RL-scenario
            sibling->color = BLACK;
            red_nephew->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_RightRotate(sibling);
            BVSBranch_LeftRotate(branch->parent);
            return;
        }
    }
    //black sibling with black nephews scenario
    sibling->color = RED;
    if (branch->parent->color == RED) {
        branch->parent->color = BLACK;
        return;
    }
    //fprintf(stderr, "Black sibling and black nephews, recurring...\n");
    Help_RmDoubleBlack(branch->parent);
    return;
}

void BVSBranch_DeleteResolve(BVSBranch *branch) {
    if (branch->color == RED) { //if red node removed, nothing to do
        //fprintf(stderr, "Red node removed, nothing to resolve\n");
        return;
    }
    BVSBranch *red_son = NULL;
    if (branch->left != NULL && branch->left->color == RED) {
        red_son = branch->left;
    }
    if (branch->right != NULL && branch->right->color == RED) {
        red_son = branch->right;
    }
    if (red_son == NULL) { //for no descendants
        //fprintf(stderr, "No red sons, we hit doubleblack situation...\n");
        Help_RmDoubleBlack(branch); //in certain scenario, recur for its parent
        return;          
    }
    //fprintf(stderr, "Resolved by re-coloring the red son\n");
    red_son->color = BLACK; //for one (obviously RED) descendant
    return;
}

BVSBranch *BVSBranch_Delete(BVSBranch *branch, char *key) {
    
    BVSBranch *current = branch;
    BVSBranch *next = branch;
    
    while (next != NULL && strcmp(next->key, key) != 0) {
        current = next;
        if (strcmp(key, current->key) > 0) {
            next = current->right;
        } else {
            next = current->left;
        }
    }
    current = next;
    if (current == NULL) {
        return branch; //no changes if not found
    }
    
    if (current->left == NULL && current->right == NULL) { //just delete the node if it has no successors
        //fprintf(stderr, "Deleting a leaf\n");
        BVSBranch *tmp_father = current->parent;
        if (tmp_father != NULL) {
            if (tmp_father->left == current) { //here we NULLify some of parent's nodes
                tmp_father->left = NULL;
            } else {
                tmp_father->right = NULL;
            }
        } else {
            branch = NULL; //crutch so that we can delete a root
        }
        //fprintf(stderr, "Unlinked the leaf, preparing to resolve...\n");
        BVSBranch_DeleteResolve(current); //to resolve the deletion we should act as if the node is still there
        free(current->key);
        free(current->data);
        free(current);
        return BVSBranch_Root(branch); //return a new root (fartherst ancestor)
    }
    
    if (current->left == NULL || current->right == NULL) { //exactly one successor
        //fprintf(stderr, "Deleting an internal with one child\n");
        BVSBranch *not_null = (current->left == NULL) ? current->right : current->left;
        not_null->parent = current->parent;
        if (!BVSBranch_IsRoot(current)) {
            if (current->parent->left == current) {
                current->parent->left = not_null;
            } else {
                current->parent->right = not_null;
            }
        } else {
            branch = not_null; //crutch for the root so we can free it
        }
        //fprintf(stderr, "Unlinked the node, preparing to resolve...\n");
        BVSBranch_DeleteResolve(current);
        free(current->key);
        free(current->data);
        free(current);
        return BVSBranch_Root(branch); //return a new root (fartherst ancestor)
    }
    //if there are two successors
    //fprintf(stderr, "Deleting an internal\n");
    BVSBranch *leftmost = current->right; //basically, we delete this node and copy its data to branch
    while (leftmost->left != NULL) { 
        //fprintf(stderr, "Not leftmost yet, proceeding to left...\n");
        leftmost = leftmost->left;
    }
    free(current->key);
    free(current->data); //free the data
    current->key = leftmost->key; //we replace node's data with the data of its in-order successor
    current->data = leftmost->data;
    if (current->right != leftmost) {
        leftmost->parent->left = leftmost->right; //we delete the leftmost node
    } else {
        current->right = leftmost->right;
    }
    if (leftmost->right != NULL) { //if any right successor
        leftmost->right->parent = leftmost->parent;
    }
    //fprintf(stderr, "Unlinked the leftmost, preparing to resolve...\n");
    BVSBranch_DeleteResolve(leftmost);
    free(leftmost);
    return BVSBranch_Root(branch);
}

void BVSBranch_LeftRotate(BVSBranch *branch) {
    //fprintf(stderr, "LEFTROTATE (%ld)!\n", branch->data);
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
    //fprintf(stderr, "RIGHTROTATE (%ld)!\n", branch->data);
    branch->left->parent = branch->parent;
    if (!BVSBranch_IsRoot(branch)) { 
        if (branch->parent->left == branch) {
            branch->parent->left = branch->left;
        } else {
            branch->parent->right = branch->left;
        }
    }
    branch->parent = branch->left;
    branch->left = branch->left->right; //we assume that branch->left != NULL
    branch->parent->right = branch;
    if (branch->left != NULL) {
        branch->left->parent = branch;
    }
    return;
}

bool BVSBranch_IsRoot(BVSBranch *branch) {
    return branch->parent == NULL;
}

int BVSBranch_Height(BVSBranch *branch) {
    if (branch == NULL)
    {
        return 0;
    }
    int right = BVSBranch_Height(branch->right); 
    int left = BVSBranch_Height(branch->left);
    if (right > left)
        return right + 1;
    else return left + 1;
}

BVSBranch *BVSBranch_Root(BVSBranch *branch) { 
    BVSBranch* current = branch;
    while (current != NULL && !BVSBranch_IsRoot(current)) {
        current = current->parent;
    }
    return current;
}

// Header file definitions

int BVS_Init(BVS *bvs) {
    bvs->root = NULL;
    return 0;
}


void BVS_Delete(BVS *bvs, char *key) {
    bvs->root = BVSBranch_Delete(bvs->root, key);    
    return;
}

void BVS_Free(BVS *bvs) {
    if (bvs->root == NULL) {
        return;
    }
    BVSBranch_Free(bvs->root);
}

void BVS_Insert(BVS *bvs, char *key, void *data, size_t size) {
    if (bvs->root == NULL) { 
        BVSBranch_Init(&(bvs->root), key, data, size);
        BVSBranch_InsertResolve(bvs->root);
    } else {
        bvs->root = BVSBranch_Insert(bvs->root, key, data, size);
    }
    return;
}

void *BVS_Search(BVS *bvs, char *key) { //returns found data, if found
    //fprintf(stderr, "Searching for (%ld)...\n", data);
    return BVSBranch_Search(bvs->root, key);

}
