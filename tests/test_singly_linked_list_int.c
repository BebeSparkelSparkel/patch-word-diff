#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "singly_linked_list.h"

SLL_H(int);
SLL_C(int);

// DEFINE TESTS HERE

// Create the test suite
Suite* singly_linked_list_suite(void) {
    Suite* s;
    TCase* tc_core;
    
    s = suite_create("Singly Linked List");
    
    // Core test case
    tc_core = tcase_create("Core");
    
    // ADD TESTS HERE

    suite_add_tcase(s, tc_core);
    
    return s;
}

// Main function
int main(void) {
    int number_failed;
    Suite* s;
    SRunner* sr;
    
    s = singly_linked_list_suite();
    sr = srunner_create(s);
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

