#ifndef INTERFACE_H
#define INTERFACE_H

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define RESULT_SUCCESS 0
#define RESULT_USER_EXIT -1
#define RESULT_ERROR_UNKNOWN -999
#define admin_login "admin"
#define admin_password "admin"

int registration(sqlite3* db);

int authorization(sqlite3* db, int* target_client_id);

int disp_client(sqlite3* db, int client_id);

int update_client(sqlite3* db, int id, int what_to_update);

int update_login(sqlite3* db, char* target);

int update_password(sqlite3* db, char* target);

int update_weight(sqlite3* db, int* target);

int update_height(sqlite3* db, int* target);

int update_gender(sqlite3* db, char* target);

int delete_client(sqlite3* db, int client_id);

int make_order(sqlite3* db, int client_id);

int update_menu(sqlite3* db, int* target_menu_id, int client_id);

int disp_all_clients(sqlite3* db);

int disp_all_orders(sqlite3* db);

int delete_all(sqlite3* db);

int disp_money_period(sqlite3* db);

int disp_most_popular_menu(sqlite3* db);

int disp_sold_plans(sqlite3* db);

int update_prices(sqlite3* db);

int disp_orders_by_date(sqlite3* db);

// const char* admin_login = "admin";

// const char* admin_password = "admin";

#endif //INTERFACE_H
