#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"

int* target_client_id;

void test_authorization(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    int result = authorization(db, target_client_id);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Suite_authorization", 0, 0);
    CU_add_test(suite, "test_authorization", test_authorization);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
