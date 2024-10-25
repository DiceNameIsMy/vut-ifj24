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
void BVSBranch_Insert(BVSBranch *branch, long data, BVS *bvs);
bool BVSBranch_Search(BVSBranch *branch, long key);
void BVSBranch_Delete(BVSBranch *branch, long key, BVS *bvs); //deletion and search is by the key, data just looks strange
bool BVSBranch_IsRoot(BVSBranch *branch);

// Rotates around the *branch
void BVSBranch_LeftRotate(BVSBranch *branch, BVS *bvs);
void BVSBranch_RightRotate(BVSBranch *branch, BVS *bvs);
// Resolves the tree structure after an insert operation
void BVSBranch_InsertResolve(BVSBranch *branch, BVS *bvs);
//resolves the tree structure after a delete operation
void BVSBranch_DeleteResolve(BVSBranch *branch, BVS *bvs);
void Help_RmDoubleBlack(BVSBranch *branch, BVS *bvs); //removes doubleblack property from the node
bool BVSBranch_IsBallanced(BVSBranch *branch);
int BVSBranch_Height(BVSBranch *branch);

// Internal definitions

void BVSBranch_Init(BVSBranch **newbranch, const long data, BVS_Color color) { 
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

void BVSBranch_InsertResolve(BVSBranch *branch, BVS *bvs) {
    //TODO: perhaps we even can un-recur this...
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
        BVSBranch_InsertResolve(granpa, bvs);
        return; //quit to un-nest the ifs
    }
    //black uncle case
    if (granpa->color == RED) {
	    fprintf(stderr, "WRONG GRANPA COLOR\n");
	    exit(-1);
    } 
    if (branch == father->left && father == granpa->left) { //LL-scenario
        BVSBranch_RightRotate(granpa, bvs);
        granpa->color = RED;
        father->color = BLACK; 
    } else if (branch == father->right && father == granpa->left) { //LR-scenario
        BVSBranch_LeftRotate(father, bvs);
        BVSBranch_RightRotate(granpa, bvs);
        granpa->color = RED;
        branch->color = BLACK; 
    } else if (branch == father->left && father == granpa->right) {//RL-scenario
        BVSBranch_RightRotate(father, bvs);
        BVSBranch_LeftRotate(granpa, bvs);
        granpa->color = RED;
        branch->color = BLACK; 
    } else if (branch == father->right && father == granpa->right) {//RR-scenario
        BVSBranch_LeftRotate(granpa, bvs);
        granpa->color = RED;
        father->color = BLACK; 
    }
    return;
}

void BVSBranch_Insert(BVSBranch *branch, const long data, BVS *bvs) {
    BVSBranch *current = branch;
    BVSBranch *next = branch;
    

    while (true) {
        //fprintf(stderr, "WEEEHEEE\n");
        current = next;
        if (data < current->data) {
            next = current->left;
            if (next == NULL) {
                //fprintf(stderr, "Initializing %ld on the left of %ld\n", data, current->data);
                BVSBranch_Init(&(current->left), data, RED);
                current->left->parent = current;
                BVSBranch_InsertResolve(current->left, bvs);
                break;
            }
        } else if (data > current->data) {
            next = current->right;
            if (next == NULL) {
                //fprintf(stderr, "Initializing %ld on the right of %ld\n", data, current->data);
                BVSBranch_Init(&(current->right), data, RED);
                current->right->parent = current;
                BVSBranch_InsertResolve(current->right, bvs);
                break;
            }
        } else break;
    }
    return;
}

bool BVSBranch_Search(BVSBranch *branch, long key) {
    //TODO: un-recur this
    if (branch == NULL) {
        return false;
    }

    if (branch->data == key) {
        return true;
    }

    // Search in branches
    if (branch->data > key) {
        return BVSBranch_Search(branch->left, key);
    }
    return BVSBranch_Search(branch->right, key);
}

void Help_RmDoubleBlack(BVSBranch *branch, BVS *bvs) {
    //TODO: and yes, we can also un-recur this
    if (BVSBranch_IsRoot(branch)) {
        fprintf(stderr, "Ended up in root, stop recurring\n");
        branch->color = BLACK;
        return;
    }
    BVSBranch *sibling = (branch->parent->left == branch || branch->parent->left == branch->left || branch->parent->left == branch->right) ? branch->parent->right : branch->parent->left; //remember we nullified the father's right/left?
    //red sibling scenario
    fprintf(stderr, "Calculated the former sibling\n");
    if (sibling->color == RED) { //sibling is never NULL
        fprintf(stderr, "Sibling was red\n");
        branch->parent->color = RED; //recolor nodes
        sibling->color = BLACK;
        if (sibling->right == NULL || sibling->left == NULL)
            fprintf(stderr, "SIB-G WITH \"NULL\" CHILDREN!!!\n");
        if (branch->parent->left == sibling) { //and rotate
            fprintf(stderr, "Left scenario\n");
            sibling = sibling->right;
            BVSBranch_RightRotate(branch->parent, bvs); //Left scenario
        } else {
            fprintf(stderr, "Right scenario\n");
            sibling = sibling->left;
            BVSBranch_LeftRotate(branch->parent, bvs); //Right scenario
        }
    } //by now the red sibling scenario is transformed either to second or the third one
    //black sibling with red nephews scenario
    fprintf(stderr, "By now sibling (new one) is black\n");
    BVSBranch *red_nephew = NULL; //check for a red nephew
    int red_nephew_cnt = 0;
    if (sibling == NULL) {
        fprintf(stderr, "something terrible is going to happen...\n");
    }
    if (sibling->left != NULL && sibling->left->color == RED) {
        red_nephew = sibling->left;
        red_nephew_cnt++;
    } 
    if (sibling->right != NULL && sibling->right->color == RED) {
        red_nephew = sibling->right;
        red_nephew_cnt++;
    }
    if (red_nephew != NULL) { //there is one or two
        fprintf(stderr, "there is a red nephew\n");
        if (sibling->parent->left == sibling && (sibling->left == red_nephew || red_nephew_cnt == 2)) { //LL-scenario
            sibling->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_RightRotate(branch->parent, bvs);
            red_nephew->color = BLACK;//we don't know which nephew we paint black...
            return;
        } //THERE'S NO ELSE...
        if (sibling->parent->right == sibling && (sibling->right == red_nephew || red_nephew_cnt == 2)) { //RR-scenario
            sibling->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_LeftRotate(branch->parent, bvs);
            red_nephew->color = BLACK; //same as above
            return;
        }
        if (sibling->parent->left == sibling && sibling->right == red_nephew) { //LR-scenario
            sibling->color = BLACK;
            red_nephew->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_LeftRotate(sibling, bvs);
            BVSBranch_RightRotate(branch->parent, bvs);
            return;
        }
        if (sibling->parent->right == sibling && sibling->left == red_nephew) { //RL-scenario
            sibling->color = BLACK;
            red_nephew->color = branch->parent->color;
            branch->parent->color = BLACK;
            BVSBranch_RightRotate(sibling, bvs);
            BVSBranch_LeftRotate(branch->parent, bvs);
            return;
        }
    }
    //black sibling with black nephews scenario
    sibling->color = RED;
    if (branch->parent->color == RED) {
        branch->parent->color = BLACK;
        return;
    }
    fprintf(stderr, "Black sibling and black nephews, recurring...\n");
    Help_RmDoubleBlack(branch->parent, bvs);
    return;
}

void BVSBranch_DeleteResolve(BVSBranch *branch, BVS *bvs) {
    if (branch->color == RED) { //if red node removed, nothing to do
        fprintf(stderr, "Red node removed, nothing to resolve\n");
        return;
    }
    /*BVSBranch *sibling = (branch->parent->left == branch || branch->parent->left == branch->left || branch->parent->left == branch->right) ? branch->parent->right : branch->parent->left; //remember we nullified the father's right/left?
    if (sibling == NULL) {
        fprintf(stderr, "We have a black node without any siblings\n");
    }*/
    BVSBranch *red_son = NULL;
    if (branch->left != NULL && branch->left->color == RED) {
        red_son = branch->left;
    }
    if (branch->right != NULL && branch->right->color == RED) {
        red_son = branch->right;
    }
    if (red_son == NULL) { //for no descendants
        fprintf(stderr, "No red sons, we hit doubleblack situation...\n");
        Help_RmDoubleBlack(branch, bvs); //in certain scenario, recur for its parent
        return;          
    }
    fprintf(stderr, "Resolved by re-coloring the red son\n");
    red_son->color = BLACK; //for one (obviously RED) descendant
    return;
    //what if the root is changed?!
}

void BVSBranch_Delete(BVSBranch *branch, long key, BVS *bvs) {
    if (branch == NULL)
        return;
    if (branch->data > key) { //recur until we reach the key or NULL
        BVSBranch_Delete(branch->left, key, bvs);
        return;
    } else if (branch->data < key){
        BVSBranch_Delete(branch->right, key, bvs);
        return;
    }
    //segfault because of root deletion
        /*BVSBranch *sibling = (branch->parent->left == branch) ? branch->parent->right : branch->parent->left;
        if (sibling == NULL && branch->color == BLACK) {
            fprintf(stderr, "BLACK WITH NO SIBLINGS\n");
        }*/
    if (branch->left == NULL && branch->right == NULL) { //just delete the node if it has no successors
        fprintf(stderr, "Deleting a leaf\n");
        BVSBranch *tmp_father = branch->parent; //what if father is NULL?
        //BVSBranch *tmp = branch;
        if (tmp_father != NULL) {
            if (tmp_father->left == branch) { //here we NULLify some of parent's nodes
                tmp_father->left = NULL;
            } else {
                tmp_father->right = NULL;
            }
        }
        fprintf(stderr, "Unlinked the leaf, preparing to resolve...\n");
        BVSBranch_DeleteResolve(branch, bvs); //to resolve the deletion we should act as if the node is still there
        free(branch);
        return;
    }
    if (branch->left == NULL || branch->right == NULL) { //exactly one successor
        fprintf(stderr, "Deleting an internal with one child\n");
        BVSBranch *not_null = (branch->left == NULL) ? branch->right : branch->left;
        not_null->parent = branch->parent;//what if branch is root?
        fprintf(stderr, "This far, no errors Ig..?\n");
        if (branch->parent != NULL) {
            if (branch->parent->left == branch) {
                branch->parent->left = not_null;
            } else {
                branch->parent->right = not_null;
            }
        }
        fprintf(stderr, "Unlinked the node, preparing to resolve...\n");
        BVSBranch_DeleteResolve(branch, bvs);//how do re resolve a root situation?
        free(branch);
        return;
    }
    //if there are two successors
    fprintf(stderr, "Deleting an internal\n");
    BVSBranch *leftmost = branch->right; //basically, we delete this node and copy its data to branch
    while (leftmost->left != NULL) { 
        fprintf(stderr, "Not leftmost yet, proceeding to left...\n");
        leftmost = leftmost->left;
    }
    branch->data = leftmost->data; //we replace node's data with the data of its in-order successor
    if (branch->right != leftmost) {
        leftmost->parent->left = leftmost->right; //we delete the leftmost node
    } else {
        branch->right = leftmost->right;
    }
    fprintf(stderr, "This far, no errors Ig..?\n");
    if (leftmost->right != NULL) { //if any right successor
        leftmost->right->parent = leftmost->parent;
    }
    fprintf(stderr, "Unlinked the leftmost, preparing to resolve...\n");
    BVSBranch_DeleteResolve(leftmost, bvs);
    fprintf(stderr, "before free\n");
    free(leftmost);
    fprintf(stderr, "After free\n");
    return;
}

void BVSBranch_LeftRotate(BVSBranch *branch, BVS *bvs) {
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
    if (bvs->root == branch) {
        bvs->root = branch->parent;
    }
    return;
}

void BVSBranch_RightRotate(BVSBranch *branch, BVS *bvs) {
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
    if (bvs->root == branch) {
        bvs->root = branch->parent;
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


void BVS_Delete(BVS *bvs, const long key) {
    BVSBranch_Delete(bvs->root, key, bvs);    
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
        BVSBranch_Insert(bvs->root, data, bvs);
    }
    //fprintf(stderr, "Tree height is %d\n", BVSBranch_Height(bvs->root));
}

bool BVS_Search(BVS *bvs, const long data) {
    //fprintf(stderr, "Searching for (%ld)...\n", data);
    return BVSBranch_Search(bvs->root, data);
}

bool BVS_IsBallanced(BVS *bvs) {
    return BVSBranch_IsBallanced(bvs->root);
}

bool BVSBranch_IsBallanced(BVSBranch *branch) {
    if (branch == NULL)
        return true;
    int right_height = BVSBranch_Height(branch->right);
    int left_height = BVSBranch_Height(branch->left);
    bool right_bal = BVSBranch_IsBallanced(branch->right);
    bool left_bal = BVSBranch_IsBallanced(branch->left);
    return right_bal && left_bal && (right_height - left_height >= -1) && (right_height - left_height <= 1);
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
