#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "sqlite3.h"
#include "../src/interface.h"

void test_update_prices(void) {
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    rc = update_prices(db);
    CU_ASSERT_EQUAL(rc, RESULT_SUCCESS);

    sqlite3_close(db);
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("update_prices_test_suite", 0, 0);
    CU_add_test(suite, "test_update_prices", test_update_prices);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}