#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"

void test_delete_client(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    int result1 = delete_client(db, 1);
    CU_ASSERT_EQUAL(result1, RESULT_SUCCESS);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("suite_delete_client", 0, 0);
    CU_add_test(suite, "test_delete_client", test_delete_client);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
