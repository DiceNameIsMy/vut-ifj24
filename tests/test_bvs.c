//
// Created by nur on 5.10.24.
//
// Modified by oleh 26.10.24

#include <stdio.h>
#include <structs/bvs.h>
#include <time.h>
#include <stdlib.h>

#include "test_utils.h"

BVS bvs;

TEST(insert_1)
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    if (BVS_Search(&bvs, 1) == NULL) {
        FAIL("Insertion and then search had failed");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(insert_2)
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (BVS_Search(&bvs, 1) == NULL) {
        FAIL("Insertion and then search had failed");
    }
    if (BVS_Search(&bvs, 2) == NULL) {
        FAIL("Insertion and then search had failed");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(insert_many)
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 10);
    BVS_Insert(&bvs, 20);
    BVS_Insert(&bvs, 3);
    BVS_Insert(&bvs, 15);
    BVS_Insert(&bvs, 0);
    BVS_Insert(&bvs, 14);
    BVS_Insert(&bvs, 12);

    if(BVS_Search(&bvs, 10) == NULL) {
	fprintf (stderr, "Failed to find (10)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 20) == NULL) {
	fprintf (stderr, "Failed to find (20)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 3) == NULL) {
	fprintf (stderr, "Failed to find (3)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 15) == NULL) {
	fprintf (stderr, "Failed to find (15)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 0) == NULL) {
	fprintf (stderr, "Failed to find (0)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 14) == NULL) {
	fprintf (stderr, "Failed to find (14)\n");
        FAIL("Not found");
    }
    if(BVS_Search(&bvs, 12) == NULL) {

	fprintf (stderr, "Failed to find (12)\n");
        FAIL("Not found");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(search_not_existing_element)
    BVS_Init(&bvs);

    if (BVS_Search(&bvs, 1) != NULL) {
        FAIL("Search on empty BVS found something");
    }

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (BVS_Search(&bvs, 3) != NULL) {
        FAIL("Search on non-empty BVS found a non-existent item");
    }
    BVS_Free(&bvs);
ENDTEST

int main() {
    BVS_Init(&bvs);

    RUN_TESTS();

    SUMMARIZE()

}
