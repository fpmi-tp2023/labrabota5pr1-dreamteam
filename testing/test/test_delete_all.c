#include <CUnit/Basic.h>
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

void test_delete_all(void)
{
    int result = delete_all(db);
    CU_ASSERT_EQUAL(result, RESULT_SUCCESS);
    sqlite3_close(db);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Suite_delete_all", init_suite, clean_suite);
    CU_add_test(suite, "test_delete_all", test_delete_all);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
