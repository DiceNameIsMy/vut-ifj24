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
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    if (!BVS_Search(&bvs, 1)) {
        FAIL("Insertion and then search had failed");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(insert_2)
    BVS_Init(&bvs);

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (!BVS_Search(&bvs, 1)) {
        FAIL("Insertion and then search had failed");
    }
    if (!BVS_Search(&bvs, 2)) {
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
    if(!BVS_Search(&bvs, 10)) {
	fprintf (stderr, "Failed to find (10)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 20)) {
	fprintf (stderr, "Failed to find (20)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 3)) {
	fprintf (stderr, "Failed to find (3)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 15)) {
	fprintf (stderr, "Failed to find (15)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 0)) {
	fprintf (stderr, "Failed to find (0)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 14)) {
	fprintf (stderr, "Failed to find (14)\n");
        FAIL("Not found");
    }
    if(!BVS_Search(&bvs, 12)) {
	fprintf (stderr, "Failed to find (12)\n");
        FAIL("Not found");
    }
    //if(!BVS_IsBallanced(&bvs)) {
    //    FAIL("Ballance is corrupted!\n"); IRRELEVANT DUE TO RB TREE PROPERTIES
    //    exit(-1);
    //}

    BVS_Free(&bvs);
ENDTEST

TEST(insert_many_and_test)
    srand(time(NULL));
    BVS_Init(&bvs);
    int max = 1000000;
    long bvs_arr[1000000] = {0};
    int q = -1;
    for (int i = 0; i < max; i++) {
        bvs_arr[i] = rand()%50000000;
        //fprintf(stderr, "NEW INPUT (%ld)\n", bvs_arr[i]);
        //BVS_Insert(&bvs, i);
        BVS_Insert(&bvs, bvs_arr[i]);
        
        //if (!BVS_IsBallanced(&bvs)) {
        //    FAIL("Ballance is corrupted!\n"); IRRELEVANT DUE TO RB-TREE PROPERTIES
        //    exit(-1);
        //}
	q*=q;
    }
    for (int i = 0; i < max; i++) {
        if(!BVS_Search(&bvs, bvs_arr[i])) {
            FAIL("Not found");
	    fprintf (stderr, "Failed to find (%ld)\n", bvs_arr[i]);
           // exit(-1);
        }
    }
    for (int i = 0; i < max; i++) {
	//fprintf(stderr, "DELETING (%ld)\n", bvs_arr[i]);
	if (!BVS_Search(&bvs, bvs_arr[i])) {
	    bool found = false;
	    for (int j = 0; j < i; j++) {
		if (bvs_arr[j] == bvs_arr[i]) {
			found = true;
			break;
		}
	    }
	    if (!found) {
	        FAIL("Deleted too early");
	    }
	}
        BVS_Delete(&bvs, bvs_arr[i]);
        if (BVS_Search(&bvs, bvs_arr[i])) {
            FAIL("Failed to delete");
        }
    }
    BVS_Free(&bvs);
ENDTEST

TEST(search_not_existing_element)
    BVS_Init(&bvs);

    if (BVS_Search(&bvs, 1)) {
        FAIL("Search on empty BVS found something");
    }

    BVS_Insert(&bvs, 1);
    BVS_Insert(&bvs, 2);
    if (BVS_Search(&bvs, 3)) {
        FAIL("Search on non-empty BVS found a non-existent item");
    }
    BVS_Free(&bvs);
ENDTEST

TEST(delete_a_node)
    
ENDTEST

int main() {
    BVS_Init(&bvs);

    RUN_TESTS();

    SUMMARIZE()

}
