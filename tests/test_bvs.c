//
// Created by nur on 5.10.24.
//

#include <stdio.h>
#include <structs/bvs.h>

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

int main() {
    BVS_Init(&bvs);

    RUN_TESTS();

    SUMMARIZE()
}
