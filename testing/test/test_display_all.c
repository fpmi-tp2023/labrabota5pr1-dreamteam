#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <sqlite3.h>
#include "../src/interface.h"

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

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("display_all_suite", init_suite, clean_suite);
    CU_add_test(suite, "test_display_all", test_display_all);
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
