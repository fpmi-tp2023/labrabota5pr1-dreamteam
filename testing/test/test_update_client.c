#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"

void test_update_client(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    int result1 = update_client(db, 1, 1);
    int result2 = update_client(db, 1, 2);
    int result3 = update_client(db, 1, 3);
    int result4 = update_client(db, 1, 4);
    int result5 = update_client(db, 1, 5);
    int result6 = update_client(db, 1, 6);
    int result7 = update_client(db, 1, 7);

    CU_ASSERT_EQUAL(result1, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result2, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result3, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result4, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result5, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result6, RESULT_SUCCESS);
    CU_ASSERT_EQUAL(result7, RESULT_SUCCESS);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("suite_update_client", 0, 0);
    CU_add_test(suite, "test_update_client", test_update_client);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
