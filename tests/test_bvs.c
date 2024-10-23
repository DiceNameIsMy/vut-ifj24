//
// Created by nur on 5.10.24.
//

#include <stdio.h>
#include <structs/bvs.h>
#include <time.h>
#include <stdlib.h>

#include "test_utils.h"

BVS bvs;

TEST(insert_1)
    BVS_Free(&bvs);
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    if (!BVS_Search(&bvs, 1)) {
        FAIL("Insertion and then search had failed");
    }
ENDTEST

TEST(insert_2)
    BVS_Free(&bvs);
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (!BVS_Search(&bvs, 1)) {
        FAIL("Insertion and then search had failed");
    }
    if (!BVS_Search(&bvs, 2)) {
        FAIL("Insertion and then search had failed");
    }
ENDTEST

TEST(insert_many)
    BVS_Free(&bvs);
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 10);
    BVS_Insert(&bvs, 20);
    BVS_Insert(&bvs, 3);
    BVS_Insert(&bvs, 15);
    BVS_Insert(&bvs, 0);
    BVS_Insert(&bvs, 14);
    BVS_Insert(&bvs, 12);
    if(!BVS_Search(&bvs, 10)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 20)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 3)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 15)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 0)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 14)) {
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 12)) {
        FAIL("Not found");
    }
    //if(!BVS_IsBallanced(&bvs)) {
    //    FAIL("Ballance is corrupted!\n"); IRRELEVANT DUE TO RB TREE PROPERTIES
    //    exit(-1);
    //}

ENDTEST

TEST(insert_many_and_test)
    srand(time(NULL));
    BVS_Free(&bvs);
    BVS_Init(&bvs);
    long bvs_arr[50000] = {0};
    for (int i = 0; i < 50000; i++) {
        fprintf(stderr, "NEW INPUT (%d)\n", i);
        bvs_arr[i] = (i*i)<<4 | (i*i)>>1;
        BVS_Insert(&bvs, bvs_arr[i]);
        //BVS_Insert(&bvs, (i*i)<<4 | (i*i)>>1);
        
        //if (!BVS_IsBallanced(&bvs)) {
        //    FAIL("Ballance is corrupted!\n"); IRRELEVANT DUE TO RB-TREE PROPERTIES
        //    exit(-1);
        //}
    }
    for (int i = 0; i < 50000; i++) {
        if(!BVS_Search(&bvs, bvs_arr[i])) {
            FAIL("Not found");
           // exit(-1);
        }
    }
ENDTEST

TEST(search_not_existing_element)
    BVS_Free(&bvs);
    BVS_Init(&bvs);

    if (BVS_Search(&bvs, 1)) {
        FAIL("Search on empty BVS found something");
    }

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (BVS_Search(&bvs, 3)) {
        FAIL("Search on non-empty BVS found a non-existent item");
    }
ENDTEST

TEST(delete_a_node)
    
ENDTEST

int main() {
    BVS_Init(&bvs);

    RUN_TESTS();

    SUMMARIZE()
}
