#include <sqlite3.h>

#ifndef INTERFACE_H
#define INTERFACE_H

int registration(sqlite3* db);

int authorization(sqlite3* db);

int disp_client(sqlite3* db, int id);

int update_client(sqlite3* db);

int delete_client(sqlite3* db);

int make_order(sqlite3* db);

int disp_all_clients(sqlite3* db);

int disp_money_period(sqlite3* db);

int disp_most_popular_menu(sqlite3* db);

int disp_sold_plans(sqlite3* db);

int update_prices(sqlite3* db);

int disp_orders_by_date(sqlite3* db);


#endif //INTERFACE_H
