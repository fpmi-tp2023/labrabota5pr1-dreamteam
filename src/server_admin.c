#include "../include/interface.h"
//#include <regex.h>

static int callback(void* data, int argc, char** argv, char** azColName) {
	printf("-------------------------------------\n");
	for (int i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("-------------------------------------\n");
	return 0;
}

//int enter_correct_date(char* target)                                           �������� getline
//{
//	regex_t regex;
//	int reti = regcomp(&regex, "^[0-9]{4}-[0-9]{2}-[0-9]{2}$", 0);
//	if (reti) {
//		printf("Error when checking the correct date format\n");
//		return RESULT_ERROR_UNKNOWN;
//	}
//
//	char* date;
//	size_t bufsize = 0;
//
//	do
//	{
//		printf("Enter the date in the format YYYY-MM-DD (or press Enter to exit): ");
//		getline(&date, &bufsize, stdin);
//		if (strlen(date) == 0 || date[0] == '\n')
//		{
//			return RESULT_USER_EXIT;
//		}
//		reti = regexec(&regex, date, 0, NULL, 0);
//		if (!reti) {
//			break;
//		}
//		else if (reti == REG_NOMATCH) {
//			printf("The date does not match the format YYYY-MM-DD\n");
//		}
//		else {
//			printf("Error when checking the correct date format\n");
//			return RESULT_ERROR_UNKNOWN;
//		}
//	} while (reti == REG_NOMATCH);
//
//	strcpy_s(target, date);
//	regfree(&regex);
//	return RESULT_SUCCESS;
//}

int disp_all_clients(sqlite3* db)
{
	const char* sql = "SELECT * FROM Client;";
	if (sqlite3_exec(db, sql, callback, 0, 0) != SQLITE_OK) {
		return RESULT_ERROR_UNKNOWN;
	}
	return RESULT_SUCCESS;
}

int disp_all_orders(sqlite3* db)
{
	char* err_msg = NULL;
	const char* sql = "SELECT * FROM Orders;";

	if (sqlite3_exec(db, sql, callback, 0, &err_msg) != SQLITE_OK) {
		printf("Error when displaying orders: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int delete_all(sqlite3* db)
{
	char* err_msg;
	char answer;
	while ((answer = getchar()) != '\n' && answer != EOF) {}
	printf("Do you really want to delete all clients and orders? (y/n): ");
	answer = getchar();
	if (answer != 'y' && answer != 'Y') {
		return RESULT_USER_EXIT;
	}

	const char* sql1 = "DELETE FROM Client;";

	int rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Error when deleting clients: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	const char* sql2 = "DELETE FROM Orders;";

	rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Error when deleting orders: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

//int disp_money_period(sqlite3* db)
//{
//	char* start_date, end_date;
//	int res = enter_correct_date(start_date);
//	if (res != RESULT_SUCCESS)
//	{
//		return res;
//	}
//	res = enter_correct_date(end_date);
//	if (res != RESULT_SUCCESS)
//	{
//		return res;
//	}
//
//	char* zErrMsg = 0;
//	int rc;
//
//	char sql[100];
//
//	sprintf_s(sql, sizeof(sql), "SELECT SUM(Meal_Plan.price) AS revenue FROM Orders JOIN Meal_Plan ON Orders.plan_id = Meal_Plan.id WHERE Orders.date BETWEEN '%s' AND '%s';", start_date, end_date);
//
//	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
//	if (rc != SQLITE_OK) {
//		printf("SQL error: %s\n", zErrMsg);
//		sqlite3_free(zErrMsg);
//		return RESULT_ERROR_UNKNOWN;
//	}
//
//	return RESULT_SUCCESS;
//}

int disp_most_popular_menu(sqlite3* db)
{
	char* err_msg = 0;
	char* sql = "SELECT * FROM Menu WHERE id = (SELECT menu_id FROM Client GROUP BY menu_id ORDER BY COUNT(*) DESC LIMIT 3);";
	int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int disp_sold_plans(sqlite3* db)
{
	char* err_msg = 0;
	char* sql = "SELECT Meal_Plan.id, Meal_Plan.type, Meal_Plan.period, Meal_Plan.price, "
		"COUNT(*), SUM(Meal_Plan.price) FROM Orders JOIN Meal_Plan ON Orders.plan_id = Meal_Plan.id GROUP BY Meal_Plan.id;";
	int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}
	return RESULT_SUCCESS;
}

int update_prices(sqlite3* db)
{	
	int plan_id;
	
	printf("Enter plan id to update or 0 to update all plans (enter a non-number to exit): ");
	if (scanf_s("%d", &plan_id) == 0)
	{
		return RESULT_USER_EXIT;
	}

	char query[100];
	char* err_msg = 0;
	int rc;
	if (plan_id != 0)
	{
		sprintf_s(query, sizeof(query), "SELECT * FROM Meal_Plan WHERE id = %d", plan_id);
		rc = sqlite3_exec(db, query, callback, 0, &err_msg);

		if (rc != SQLITE_OK) {
			printf("Meal_Plan not found.\n");
			return RESULT_ERROR_UNKNOWN;
		}
	}
	

	int percent;
	do
	{
		printf("Enter positive percent to increase price or negative to decrease price"
			" (enter a non-integer to exit): ");
		if (scanf_s("%d", &percent) == 0)
		{
			return RESULT_USER_EXIT;
		}
		if (percent < -100 || percent > 100)
		{
			printf("Incorrect number. Try again\n");
		}
	} while (percent < -100 || percent > 100);

	if (plan_id == 0)
	{
		sprintf_s(query, sizeof(query), "UPDATE Meal_Plan SET price = price * (1 + (%d / 100));", percent);
	}
	else
	{
		sprintf_s(query, sizeof(query), "UPDATE Meal_Plan SET price = price * (100 + (%d / 100)) WHERE id = %d;", 
			percent, plan_id);
	}

	rc = sqlite3_exec(db, query, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("Error when trying to update prices: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}
	return RESULT_SUCCESS;
}

//int disp_orders_by_date(sqlite3* db)						
//{
//	char* date;
//	int res = enter_correct_date(date);
//	if (res != RESULT_SUCCESS)
//	{
//		return res;
//	}
//
//	char* err_msg = 0;
//	int rc;
//	char sql[100];
//	sprintf_s(sql, sizeof(sql), "SELECT * FROM Orders WHERE date = '%s'", date);
//	rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
//	if (rc != SQLITE_OK) {
//		printf("SQL error: %s\n", err_msg);
//		sqlite3_free(err_msg);
//		return RESULT_ERROR_UNKNOWN;
//	}
//
//	return RESULT_SUCCESS;
//}