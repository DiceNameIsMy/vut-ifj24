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
long data_1 = 1, data_2 = 2, data_3 = 3;

TEST(insert_1)
    BVS_Init(&bvs);
    BVS_Insert(&bvs, "one", (void *)&data_1, sizeof(long));
    if (BVS_Search(&bvs, "one") == NULL) {
        FAIL("Insertion and then search had failed");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(insert_3_and_delete)
    BVS_Init(&bvs);

    BVS_Insert(&bvs, "one", (void *)&data_1, sizeof(long));
    BVS_Insert(&bvs, "two", (void *)&data_2, sizeof(long));
    BVS_Insert(&bvs, "three", (void *)&data_3, sizeof(long));
    if (BVS_Search(&bvs, "one") == NULL) {
        FAIL("Insertion and then search had failed");
    }
    if (BVS_Search(&bvs, "two") == NULL) {
        FAIL("Insertion and then search had failed");
    }
    BVS_Delete(&bvs, "two");
    BVS_Delete(&bvs, "three");
    BVS_Free(&bvs);
ENDTEST

/*TEST(insert_many)
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
ENDTEST*/

TEST(search_not_existing_element)
    BVS_Init(&bvs);

    if (BVS_Search(&bvs, "one hundred fourty six") != NULL) {
        FAIL("Search on empty BVS found something");
    }

    BVS_Insert(&bvs, "one", (void *)&data_1, sizeof(long));
    BVS_Insert(&bvs, "two", (void *)&data_2, sizeof(long));
    if (BVS_Search(&bvs, "three") != NULL) {
        FAIL("Search on non-empty BVS found a non-existent item");
    }
    BVS_Free(&bvs);
ENDTEST

int main() {
    BVS_Init(&bvs);

    RUN_TESTS();

    SUMMARIZE()

}
