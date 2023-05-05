#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"

void test_disp_client_menu(void)
{
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    int result1 = disp_client_menu(db, 1);
    CU_ASSERT_EQUAL(result1, RESULT_SUCCESS);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("suite_disp_client_menu", 0, 0);
    CU_add_test(suite, "test_disp_client_menu", test_disp_client_menu);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
