#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <CUnit/Basic.h>
#include "../src/interface.h"

void test_enter_correct_date_valid_format(void) {
    char* input = "2023-05-05";
    char* output = NULL;
    int result = enter_correct_date(&output);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
    CU_ASSERT_STRING_EQUAL(input, output);
    free(output);
}

void test_enter_correct_date_invalid_format(void) {
    char* input = "invalid date";
    char* output = NULL;
    int result = enter_correct_date(&output);
    CU_ASSERT_EQUAL(result, RESULT_ERROR_UNKNOWN);
    CU_ASSERT_PTR_NULL(output);
    free(output);
}

void test_enter_correct_date_empty_input(void) {
    char* output = NULL;
    int result = enter_correct_date(&output);
    CU_ASSERT_EQUAL(result, RESULT_USER_EXIT);
    CU_ASSERT_PTR_NULL(output);
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("enter_correct_date_suite", NULL, NULL);

    CU_add_test(suite, "test_enter_correct_date_valid_format", test_enter_correct_date_valid_format);
    CU_add_test(suite, "test_enter_correct_date_invalid_format", test_enter_correct_date_invalid_format);
    CU_add_test(suite, "test_enter_correct_date_empty_input", test_enter_correct_date_empty_input);

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_basic_run_tests();

    CU_cleanup_registry();

    return 0;
}
