#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "sqlite3.h"
#include "../src/interface.h"

int callback(void* data, int argc, char** argv, char** azColName) {
    return 0;
}

void test_disp_sold_plans(void) {
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    rc = disp_sold_plans(db);
    CU_ASSERT_EQUAL(rc, RESULT_SUCCESS);

    sqlite3_close(db);
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("disp_sold_plans_test_suite", 0, 0);
    CU_add_test(suite, "test_disp_sold_plans", test_disp_sold_plans);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
