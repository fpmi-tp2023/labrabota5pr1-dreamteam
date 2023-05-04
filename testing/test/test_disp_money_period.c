#include <CUnit/Basic.h>
#include "../src/interface.h"
#include "sqlite3.h"

int init_suite(void) {
   return 0;
}

int cleanup_suite(void) {
   return 0;
}

void test_disp_money_period(void) {
   sqlite3* db;
   int rc = sqlite3_open("src/test.db", &db);
   CU_ASSERT_EQUAL(rc, SQLITE_OK);

   int result = disp_money_period(db);
   CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
}

CU_TestInfo tests_disp_money_period[] = {
   { "test_disp_money_period", test_disp_money_period },
   CU_TEST_INFO_NULL,
};

int main() {
   CU_initialize_registry();
   CU_pSuite suite = CU_add_suite("Suite", init_suite, cleanup_suite);
   CU_add_test(suite, tests_disp_money_period[0].pName, tests_disp_money_period[0].pTestFunc);
   CU_basic_run_tests();
   CU_cleanup_registry();

   return 0;
}
