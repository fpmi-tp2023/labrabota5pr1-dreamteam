#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"

void test_delete_all(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);
    int result = delete_all(db);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
    sqlite3_close(db);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Suite_delete_all", 0, 0);
    CU_add_test(suite, "test_delete_all", test_delete_all);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
