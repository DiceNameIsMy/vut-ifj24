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
