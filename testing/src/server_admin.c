#define _CRT_SECURE_NO_WARNINGS
#include "interface.h"
#include <regex.h>

static int callback(void* data, int argc, char** argv, char** azColName) {

	for (int i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("-------------------------------------\n");
	return 0;
}

int enter_correct_date(char** target)
{
	regex_t regex;
	int reti = regcomp(&regex, "^[0-9]{4}-[0-9]{2}-[0-9]{2}$", 0);
	if (reti) {
		print_error_prompt("Error when checking the correct date format");
		return RESULT_ERROR_UNKNOWN;
	}

	char* date = calloc(100, sizeof(char));

	do
	{
		printf("Enter the date in the format YYYY-MM-DD (or press Enter to exit): ");
		fgets(date, 100, stdin);

		if (date[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (date[strlen(date) - 1] == '\n')
			date[strlen(date) - 1] = 0;
		
		reti = regexec(&regex, date, 0, NULL, 0);

		if (!reti) {
			break;
		}
		else if (reti == REG_NOMATCH) {
			print_error_prompt("The date does not match the format YYYY-MM-DD");
		}
		else {
			print_error_prompt("Error when checking the correct date format");
			return RESULT_ERROR_UNKNOWN;
		}

	} while (reti == REG_NOMATCH);

	*target = date;
	regfree(&regex);
	return RESULT_SUCCESS;
}

int display_all(sqlite3* db, int what_to_display)
{
	char* query;
	char* err_msg = NULL;
	int rc;
	switch (what_to_display)
	{
	case (1):
		query = sqlite3_mprintf("SELECT * FROM Client;");
		break;
	case (2):
		query = sqlite3_mprintf("SELECT Orders.*, Meal_Plan.type, Meal_Plan.period, Meal_Plan.price "
		"FROM Orders JOIN Meal_Plan ON Orders.plan_id = Meal_Plan.id;");
		break;
	case (3):
		query = sqlite3_mprintf("SELECT id, type, period, price FROM Meal_Plan;");
		break;
	case (4):
		query = sqlite3_mprintf("SELECT * FROM Menu;");
		break;
	default:
		print_error_prompt("Option not found.");
		return RESULT_ERROR_UNKNOWN;
	}

	printf("-------------------------------------\n");
    rc = sqlite3_exec(db, query, callback, 0, &err_msg);
	if (rc != SQLITE_OK) {
		system("cls");
		printf("Error when displaying data: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int delete_all(sqlite3* db)
{
	char* err_msg;
	char answer;
	printf("Do you really want to delete all clients and orders? (y/n): ");
	answer = getchar();
	if (answer != 'y' && answer != 'Y') {
		return RESULT_USER_EXIT;
	}

	const char* sql1 = "DELETE FROM Client;";

	int rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		system("cls");
		printf("Error when deleting clients: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	const char* sql2 = "DELETE FROM Orders;";

	rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		system("cls");
		printf("Error when deleting orders: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int disp_money_period(sqlite3* db)
{
	char* start_date = NULL;
	char* end_date = NULL;
	int res = enter_correct_date(&start_date);
	if (res != RESULT_SUCCESS)
	{
		return res;
	}
	res = enter_correct_date(&end_date);
	if (res != RESULT_SUCCESS)
	{
		free(start_date);
		return res;
	}

	char* zErrMsg = 0;
	int rc;

	char sql[100];

	sprintf(sql, "SELECT SUM(Meal_Plan.price) AS Revenue FROM Orders JOIN Meal_Plan ON Orders.plan_id = Meal_Plan.id "
	"WHERE Orders.date BETWEEN '%s' AND '%s';", start_date, end_date);

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		system("cls");
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return RESULT_ERROR_UNKNOWN;
	}

	free(start_date);
	free(end_date);
	return RESULT_SUCCESS;
}

int disp_most_popular_menu(sqlite3* db)
{
	char* err_msg = 0;
	char* sql = "SELECT m.*, COUNT(c.menu_id) as Clients_amount "
	"FROM Menu m "
	"JOIN Client c ON m.id = c.menu_id "
	"GROUP BY m.id "
	"ORDER BY Clients_amount DESC "
	"LIMIT 3";
	printf("-------------------------------------\n");
	int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

	if (rc != SQLITE_OK) {
		system("cls");
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int disp_sold_plans(sqlite3* db)
{
	char* err_msg = 0;
	char* sql = sqlite3_mprintf("SELECT Meal_Plan.id, Meal_Plan.type, Meal_Plan.period, Meal_Plan.price, "
		"COUNT(*) as Amount_Sold, SUM(Meal_Plan.price) as Total_Sum FROM Orders JOIN Meal_Plan ON "
		"Orders.plan_id = Meal_Plan.id GROUP BY Meal_Plan.id;");
	printf("-------------------------------------\n");
	int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

	if (rc != SQLITE_OK) {
		system("cls");
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(sql);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_stmt* stmt;
	sql = sqlite3_mprintf("SELECT COUNT(*), SUM(price) FROM "
	"(SELECT price FROM Orders JOIN Meal_Plan ON Orders.plan_id = Meal_plan.id) subquery;");
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to information about sold plans: %s\n", sqlite3_errmsg(db));
		sqlite3_free(sql);
		sqlite3_finalize(stmt);
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		printf("Total amount sold: %d\n", sqlite3_column_int(stmt, 0));
		printf("Total sum received: %.2f\n", sqlite3_column_double(stmt, 1));
		printf("-------------------------------------\n");
	}
	else {
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(sql);
		sqlite3_finalize(stmt);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(sql);
	sqlite3_finalize(stmt);
	return RESULT_SUCCESS;
}

int update_prices(sqlite3* db)
{	
	int plan_id;
	char* query = NULL;
	int rc;
	sqlite3_stmt* stmt;
	do
	{
		system("cls");
		printf("Enter plan id to update or 0 to update all plans (enter a non-number to exit): ");
		if (scanf("%d", &plan_id) == 0)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF) {};
			return RESULT_USER_EXIT;
		}
		getchar();

		if (plan_id != 0)
		{
			query = sqlite3_mprintf("SELECT * FROM Meal_Plan WHERE id = %d", plan_id);
			rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

			if (rc != SQLITE_OK) {
				printf("Error when trying to update prices: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_free(query);
				return RESULT_ERROR_UNKNOWN;
			}

			rc = sqlite3_step(stmt);
			

			if (rc != SQLITE_ROW) {
				print_error_prompt("Plan not found.");
				sqlite3_finalize(stmt);
			}
			
			sqlite3_free(query);
		}
	} while (rc != SQLITE_ROW);
	
	int percent;
	do
	{
		system("cls");
		printf("Plan to be updated:\n");
		printf("------------------------------------------\n");
		printf("Type: %s\n", sqlite3_column_text(stmt, 1));
		printf("Period: %d mon.\n", sqlite3_column_int(stmt, 2));
		printf("Price: %.2f\n", sqlite3_column_double(stmt, 3));
		printf("------------------------------------------\n");
		printf("Enter positive percent to increase price or negative to decrease price"
			" (enter a non-integer to exit): ");
		if (scanf("%d", &percent) == 0)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF) {};
			return RESULT_USER_EXIT;
		}
		getchar();
		if (percent < -100 || percent > 100)
		{
			print_error_prompt("Incorrect number. Try again");
		}
		system("cls");
	} while (percent < -100 || percent > 100);

	if (plan_id == 0)
	{
		query = sqlite3_mprintf("UPDATE Meal_Plan SET price = price * (1 + (%d * 1.0 / 100));", percent);
	}
	else
	{
		query = sqlite3_mprintf("UPDATE Meal_Plan SET price = price * (1 + (%d * 1.0 / 100)) WHERE id = %d;", 
			percent, plan_id);
	}

	char* err_msg = NULL;
	rc = sqlite3_exec(db, query, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		system("cls");
		printf("Error when trying to update prices: %s\n", err_msg);
		sqlite3_free(query);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	sqlite3_finalize(stmt);
	return RESULT_SUCCESS;
}

int disp_orders_by_date(sqlite3* db)						
{
	char* date = NULL;
	int res = enter_correct_date(date);
	if (res != RESULT_SUCCESS)
	{
		return res;
	}

	char* err_msg = 0;
	int rc;
	char sql[100];
	sprintf(sql, "SELECT * FROM Orders WHERE date = '%s'", date);
	rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
	if (rc != SQLITE_OK) {
		print_error_prompt(err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	free(date);
	return RESULT_SUCCESS;
}