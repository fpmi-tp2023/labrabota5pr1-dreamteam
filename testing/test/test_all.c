#include <CUnit/Basic.h>
#include <sqlite3.h>
#include "../src/interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

sqlite3* db;

int init_suite(void) {
    int rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // create tables and insert test data
    const char* create_sql =
        "CREATE TABLE Client (id INTEGER PRIMARY KEY, name TEXT, address TEXT);"
        "CREATE TABLE Meal_Plan (id INTEGER PRIMARY KEY, type TEXT, period INTEGER, price REAL);"
        "CREATE TABLE Menu (id INTEGER PRIMARY KEY, name TEXT, price REAL);"
        "CREATE TABLE Orders (id INTEGER PRIMARY KEY, client_id INTEGER, plan_id INTEGER, FOREIGN KEY(client_id) REFERENCES Client(id), FOREIGN KEY(plan_id) REFERENCES Meal_Plan(id));"
        "INSERT INTO Client (name, address) VALUES ('John Doe', '123 Main St');"
        "INSERT INTO Meal_Plan (type, period, price) VALUES ('Basic', 7, 50.0), ('Standard', 14, 100.0), ('Premium', 30, 200.0);"
        "INSERT INTO Menu (name, price) VALUES ('Hamburger', 5.0), ('Pizza', 8.0), ('Salad', 3.0);"
        "INSERT INTO Orders (client_id, plan_id) VALUES (1, 1), (1, 2), (1, 3);";
    char* err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create tables and insert data: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    }

    return 0;
}

int clean_suite(void) {
    sqlite3_close(db);
    return 0;
}

void test_delete_all(void)
{
    int result = delete_all(db);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
    sqlite3_close(db);
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

void test_disp_most_popular_menu(void) {
   sqlite3* db;
   int rc = sqlite3_open("src/test.db", &db);
   CU_ASSERT_EQUAL(rc, SQLITE_OK);

   int result = disp_most_popular_menu(db);
   CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
}

CU_TestInfo tests_disp_most_popular_menu[] = {
   { "test_disp_most_popular_menu", test_disp_most_popular_menu },
   CU_TEST_INFO_NULL,
};

void test_disp_orders_by_date(void) {
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    rc = disp_orders_by_date(db);
    CU_ASSERT_EQUAL(rc, RESULT_SUCCESS);

    sqlite3_close(db);
}

void test_disp_sold_plans(void) {
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    rc = disp_sold_plans(db);
    CU_ASSERT_EQUAL(rc, RESULT_SUCCESS);

    sqlite3_close(db);
}

void test_display_all(void) {
    // Test displaying all clients
    int result = display_all(db, 1);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);

    // Test displaying all orders
    result = display_all(db, 2);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);

    // Test displaying all meal plans
    result = display_all(db, 3);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);

    // Test displaying all menu items
    result = display_all(db, 4);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);

    // Test displaying invalid option
    result = display_all(db, 5);
    CU_ASSERT_EQUAL(result, RESULT_ERROR_UNKNOWN);
}

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


void test_update_prices(void) {
    sqlite3* db;
    int rc = sqlite3_open("src/test.db", &db);
    CU_ASSERT_EQUAL(rc, SQLITE_OK);

    rc = update_prices(db);
    CU_ASSERT_EQUAL(rc, RESULT_SUCCESS);

    sqlite3_close(db);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite1 = CU_add_suite("Suite_delete_all", init_suite, clean_suite);
    CU_add_test(suite1, "test_delete_all", test_delete_all);
    CU_pSuite suite2 = CU_add_suite("Suite", 0, 0);
    CU_add_test(suite2, tests_disp_money_period[0].pName, tests_disp_money_period[0].pTestFunc);
    CU_pSuite suite3 = CU_add_suite("Suite", 0, 0);
    CU_add_test(suite3, tests_disp_most_popular_menu[0].pName, tests_disp_most_popular_menu[0].pTestFunc);
    CU_pSuite suite4 = CU_add_suite("disp_orders_by_date_test_suite", 0, 0);
    CU_add_test(suite4, "test_disp_orders_by_date", test_disp_orders_by_date);
    CU_pSuite suite5 = CU_add_suite("disp_sold_plans_test_suite", 0, 0);
    CU_add_test(suite5, "test_disp_sold_plans", test_disp_sold_plans);
    CU_pSuite suite6 = CU_add_suite("display_all_suite", init_suite, clean_suite);
    CU_add_test(suite6, "test_display_all", test_display_all);
    CU_pSuite suite7 = CU_add_suite("enter_correct_date_suite", NULL, NULL);

    CU_add_test(suite7, "test_enter_correct_date_valid_format", test_enter_correct_date_valid_format);
    CU_add_test(suite7, "test_enter_correct_date_invalid_format", test_enter_correct_date_invalid_format);
    CU_add_test(suite7, "test_enter_correct_date_empty_input", test_enter_correct_date_empty_input);

    CU_pSuite suite8 = CU_add_suite("update_prices_test_suite", 0, 0);
    CU_add_test(suite8, "test_update_prices", test_update_prices);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    CU_cleanup_registry();

    return CU_get_error();
}