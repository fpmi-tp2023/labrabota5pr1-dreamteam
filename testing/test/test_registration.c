#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"



void test_registration(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    int result = registration(db);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
    sqlite3_close(db);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Suite_registration", 0, 0);
    CU_add_test(suite, "test_registration", test_registration);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
