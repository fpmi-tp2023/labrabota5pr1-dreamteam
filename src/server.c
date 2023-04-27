#define _CRT_SECURE_NO_WARNINGS
#include "../include/interface.h"

int callback_client_id(void* client_id, int argc, char** argv, char** column_name) {
	for (int i = 0; i < argc; i++)
	{
		if (column_name[i] == "id")
		{
			*((int*)client_id) = atoi(argv[i]);
			break;
		}
	}
	return 0;
}

int callback_height(void* height, int argc, char** argv, char** column_name) {
	for (int i = 0; i < argc; i++)
	{
		if (column_name[i] == "height")
		{
			*((int*)height) = atoi(argv[i]);
			break;
		}
	}
	return 0;
}

int callback_weight(void* weight, int argc, char** argv, char** column_name) {
	for (int i = 0; i < argc; i++)
	{
		if (column_name[i] == "weight")
		{
			*((int*)weight) = atoi(argv[i]);
			break;
		}
	}
	return 0;
}

int registration(sqlite3* db)
{
	char* err_msg = 0;
	int rc;
	char sql_query[200];


	char* login = NULL;
	if (update_login(db, login) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	char* password = NULL;
	if (update_password(db, password) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	char* gender = NULL;
	if (update_gender(db, gender) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	int weight = 0;
	if (update_weight(db, &weight) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	int height = 0;
	if (update_height(db, &height) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	float bmi = (float)weight / (height * height);

	sprintf(sql_query, "INSERT INTO Client VALUES ('%s', '%s', '%s', '%d', '%d', '%f');",
		login, password, gender, weight, height, bmi);

	free(login);
	free(password);
	free(gender);
	rc = sqlite3_exec(db, sql_query, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("Failed to add a user: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int authorization(sqlite3* db, int* target_client_id)
{
	char* err_msg = 0;
	int rc;
	char sql_query[200];

	char* login = malloc(100 * sizeof(char));
	char* password = malloc(100 * sizeof(char));
	int client_id = -1;

	do
	{
		memset(login, 0, strlen(login));
		memset(password, 0, strlen(password));

		printf("Enter your login (or press Enter to exit): ");
		fgets(login, 100, stdin);

		if (login[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (login[strlen(login) - 1] == '\n')
			login[strlen(login) - 1] = 0;

		printf("Enter your password (or press Enter to exit): ");
		fgets(password, 100, stdin);

		if (password[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (password[strlen(password) - 1] == '\n')
			password[strlen(password) - 1] = 0;

		if (strcmp(login, ADMIN_LOGIN) == 0 && strcmp(password, ADMIN_PASSWORD) == 0)
		{
			*target_client_id = 0;
			return RESULT_SUCCESS;
		}

		sprintf(sql_query, "SELECT login FROM Client WHERE login = '%s' AND password = '%s';", login, password);
		rc = sqlite3_exec(db, sql_query, callback_client_id, &client_id, &err_msg);
		if (rc != SQLITE_OK) {
			printf("Error when trying to log in to the account: %s\n", err_msg);
		}
		if (client_id == -1) {
			printf("Incorrect login or password.\n");
		}
	} while (rc != SQLITE_OK || client_id == -1);

	if (err_msg != NULL)
	{
		sqlite3_free(err_msg);
	}

	*target_client_id = client_id;
	free(login);
	free(password);
	return RESULT_SUCCESS;
}

int disp_client(sqlite3* db, int id)
{
	char* query = sqlite3_mprintf("SELECT login, gender, weight, height, bmi, plan_id, "
		"menu_id FROM Client WHERE id = %d", id);

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display information: %s\n", sqlite3_errmsg(db));
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		printf("Hello, %s\n", sqlite3_column_text(stmt, 0));
		printf("Gender: %s\n", sqlite3_column_text(stmt, 1));
		printf("Current weight: %d\n", sqlite3_column_int(stmt, 2));
		printf("Current height: %d\n", sqlite3_column_int(stmt, 3));
		printf("Current BMI: %.3f\n", sqlite3_column_double(stmt, 4));
		int plan_id = sqlite3_column_int(stmt, 5);
		if (plan_id == 0)
		{
			printf("No meal plan yet.\n");
		}
		else
		{
			float bmi = sqlite3_column_double(stmt, 4);
			int menu_id = sqlite3_column_int(stmt, 6);

			sqlite3_free(query);
			sqlite3_finalize(stmt);

			query = sqlite3_mprintf("SELECT type, period FROM Meal_Plan WHERE p.id = %d", plan_id);		
			int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc != SQLITE_OK) {
				printf("Error when trying to display meal plan information: %s\n", sqlite3_errmsg(db));
				return RESULT_ERROR_UNKNOWN;
			}
			rc = sqlite3_step(stmt);
			if (rc == SQLITE_ROW)
			{
				printf("Current plan: %s %d mon.\n", sqlite3_column_text(stmt, 0), sqlite3_column_int(stmt, 1));
			}

			sqlite3_free(query);
			sqlite3_finalize(stmt);

			query = sqlite3_mprintf("SELECT m.breakfast, m.lunch, m.dinner, m.calories, "
				"m.proteins, m.fats, m.carbohydrates from Menu WHERE id = %i", menu_id);
			rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc != SQLITE_OK) {
				printf("Error when trying to display menu information: %s\n", sqlite3_errmsg(db));
				return RESULT_ERROR_UNKNOWN;
			}
			rc = sqlite3_step(stmt);
			if (rc == SQLITE_ROW)
			{
				printf("Today's menu:\n");
				printf("Breakfast - %s\n", sqlite3_column_text(stmt, 2));
				printf("Lunch - %s\n", sqlite3_column_text(stmt, 3));
				printf("Dinner - %s\n", sqlite3_column_text(stmt, 4));
				printf("%d cal., %d pr., %d fat., %d carb.\n",
					sqlite3_column_int(stmt, 5), sqlite3_column_int(stmt, 6), 
					sqlite3_column_int(stmt, 7), sqlite3_column_int(stmt, 8));
			}

			sqlite3_free(query);
			sqlite3_finalize(stmt);

			query = sqlite3_mprintf("SELECT * FROM Meal_Plan WHERE id=? AND ? BETWEEN min_bmi AND max_bmi");
			rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, id);
				sqlite3_bind_double(stmt, 2, bmi);
			}
			else {
				printf("Error when trying to display meal plan recommendations: %s\n", sqlite3_errmsg(db));
				return RESULT_ERROR_UNKNOWN;
			}

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW)
			{
				printf("It is recommended that you change your meal plan");
			}
		}
	}
	else {
		printf("User not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_finalize(stmt);
	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int update_client(sqlite3* db, int id, int what_to_update)
{
	void* target = NULL;
	char* query;
	char* err_msg = NULL;
	int rc;
	switch (what_to_update)
	{
	case (1):
		if (update_menu(db, (int*)target, id) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET menu_id = '%d' WHERE id = %d", *((int*)target), id);
		break;
	case (2):
		if (update_weight(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		int height = -1;
		query = sqlite3_mprintf("SELECT height FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_height, &height, &err_msg);
		if (rc != SQLITE_OK || height == -1) {
			printf("Error when updating weight: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET weight = '%s', bmi = '%f' WHERE id = %d",
			*((int*)target), (float)*((int*)target) / (height * height), id);
		break;
	case (3):
		if (update_height(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		int weight = -1;
		query = sqlite3_mprintf("SELECT weight FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_weight, &weight, &err_msg);
		if (rc != SQLITE_OK || weight == -1) {
			printf("Error when updating height: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET height = '%s', bmi = '%f' WHERE id = %d",
			*((int*)target), weight / ((*((int*)target)) * (*((int*)target))), id);
		break;
	case (4):
		if (update_gender(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET gender = '%s' WHERE id = %d", (char*)target, id);
		free(target);
		break;
	case (5):
		if (update_login(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET login = '%s' WHERE id = %d", (char*)target, id);
		free(target);
		break;
	case (6):
		if (update_password(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET password = '%s' WHERE id = %d", (char*)target, id);
		free(target);
		break;
	default:
		printf("Option not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

    rc = sqlite3_exec(db, query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Error when updating data: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int delete_client(sqlite3* db, int id)
{
	char answer;
	printf("Do you really want to delete your account? (y/n): ");
	scanf("%c", &answer);
	if (answer != 'y' && answer != 'Y') {
		return RESULT_USER_EXIT;
	}

	char* query = sqlite3_mprintf("DELETE FROM Client WHERE id = %d", id);

	char* err_msg = NULL;
	int rc = sqlite3_exec(db, query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Error when deleting an account: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int make_order(sqlite3* db, int client_id)
{
	char* query;
	char* err_msg = NULL;
	query = sqlite3_mprintf("SELECT bmi, plan_id FROM Client WHERE id = %d", client_id);

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display the proposed plans: %s\n", sqlite3_errmsg(db));
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	float bmi;
	int plan_cur_id;
	if (rc == SQLITE_ROW) {
		bmi = sqlite3_column_double(stmt, 0);
		plan_cur_id = sqlite3_column_int(stmt, 1);
	}
	else {
		printf("User not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_finalize(stmt);
	sqlite3_free(query);
	query = sqlite3_mprintf("SELECT id, type, period, price FROM Meal_Plan WHERE min_bmi <= %f AND max_bmi > %f"
		" AND id != %d", bmi, bmi, plan_cur_id);
	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display the proposed plans: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	int max_periods_amount = 3;
	int* plans_id = (int*)calloc(max_periods_amount, sizeof(int));
	printf("Available meal plans for your BMI:\n");
	printf("ID\tType\tPeriod(mon.)\tPrice\n");

	for (int i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++) {
		plans_id[i] = sqlite3_column_int(stmt, 0);
		printf("%d\t%s\t%d\t$%.2f\n", plans_id[i], sqlite3_column_text(stmt, 1),
			sqlite3_column_int(stmt, 2), sqlite3_column_double(stmt, 3));
	}

	int usr_choice;
	int found = 0;
	do
	{
		printf("Enter the ID of the meal plan you want to order (enter a non-number to exit): ");
		if (scanf("%d", &usr_choice) == 0)
		{
			return RESULT_USER_EXIT;
		}
		for (int i = 0; i < max_periods_amount && plans_id[i] != 0; i++)
		{
			if (usr_choice == plans_id[i])
			{
				found = 1;
				break;
			}
		}
		if (found == 0)
		{
			printf("Incorrect number. Try again\n");
		}
	} while (found == 0);

	sqlite3_finalize(stmt);
	sqlite3_free(query);

	time_t now = time(NULL);
	char datestr[20];
	strftime(datestr, sizeof(datestr), "%Y-%m-%d", localtime(&now));

	query = sqlite3_mprintf("INSERT INTO Orders VALUES('%d', '%s', '%d'); ",
		client_id, datestr, usr_choice);
	rc = sqlite3_exec(db, query, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("Failed to process an order: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	int menu_id = 0;
	if (update_menu(db, &menu_id, client_id) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	sqlite3_free(query);
	query = sqlite3_mprintf("UPDATE Client SET plan_id = '%d', menu_id = '%d' WHERE id = %d", 
		usr_choice, menu_id, client_id);

	rc = sqlite3_exec(db, query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Error when updating plan: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	free(plans_id);
	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int update_menu(sqlite3* db, int* target_menu_id, int client_id)
{
	char* query;
	char* err_msg = NULL;
	query = sqlite3_mprintf("SELECT plan_id, menu_id FROM Client WHERE id = %d", client_id);

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to read information: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		int plan_id = sqlite3_column_int(stmt, 0);
		int menu_id = sqlite3_column_int(stmt, 1);
		sqlite3_free(query);
		sqlite3_finalize(stmt);
		query = sqlite3_mprintf("SELECT m.id, m.breakfast, m.lunch, m.dinner, m.calories, m.proteins, "
			"m.fats, m.carbohydrates FROM Menu m JOIN Meal_Plan p on m.plan_type = p.type WHERE p.id = %d AND id != %d", 
			plan_id, menu_id);
		rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

		if (rc != SQLITE_OK) {
			printf("Error when trying to display the proposed menus: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}

		int max_menus_amount = 3;
		int* menus_id = (int*)calloc(max_menus_amount, sizeof(int));
		printf("Available menus for your plan:\n");

		for(int i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++)
		{
			menus_id[i] = sqlite3_column_int(stmt, 0);
			printf("ID - %d\n", menus_id[i]);
			printf("Breakfast - %s\n", sqlite3_column_text(stmt, 2));
			printf("Lunch - %s\n", sqlite3_column_text(stmt, 3));
			printf("Dinner - %s\n", sqlite3_column_text(stmt, 4));
			printf("%d cal., %d pr., %d fat., %d carb.\n",
				sqlite3_column_int(stmt, 5), sqlite3_column_int(stmt, 6),
				sqlite3_column_int(stmt, 7), sqlite3_column_int(stmt, 8));
			printf("------------------------------------------\n");
		}
		
		int usr_choice;
		int found = 0;
		do
		{
			printf("Enter the ID of the menu you prefer (enter a non-number to exit): ");
			if (scanf("%d", &usr_choice) == 0)
			{
				return RESULT_USER_EXIT;
			}
			for (int i = 0; i < max_menus_amount && menus_id[i] != 0; i++)
			{
				if (usr_choice == menus_id[i])
				{
					found = 1;
					break;
				}
			}
			if (found == 0)
			{
				printf("Incorrect number. Try again\n");
			}
		} while (found == 0);

		
		*target_menu_id = usr_choice;
		free(menus_id);
	}
	else {
		printf("User not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_finalize(stmt);
	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int update_login(sqlite3* db, char* target)
{
	char* err_msg = NULL;
	int rc;
	char sql_query[200];
	char* login = malloc(100 * sizeof(char));
	do
	{
		memset(login, 0, strlen(login));
		printf("Enter your login (or press Enter to exit): ");
		fgets(login, 100, stdin);

		if (login[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (login[strlen(login) - 1] == '\n')
			login[strlen(login) - 1] = 0;

		if (strcmp(login, ADMIN_LOGIN) == 0)
		{
			printf("A user with this login already exists.\n");
		}

		sprintf(sql_query, "SELECT login FROM Client WHERE login = '%s';", login);
		rc = sqlite3_exec(db, sql_query, 0, 0, &err_msg);

		if (rc == SQLITE_ROW) {
			printf("A user with this login already exists.\n");
		}

	} while (rc == SQLITE_ROW);

	if (err_msg != NULL)
	{
		sqlite3_free(err_msg);
	}

	target = malloc(strlen(login) + 1);
	strcpy(target, login);
	free(login);
	return RESULT_SUCCESS;
}

int update_password(sqlite3* db, char* target)
{
	char* password = calloc(100, sizeof(char));
	printf("Enter your password (or press Enter to exit): ");
	fgets(password, 100, stdin);

	if (password[0] == '\n')
	{
		return RESULT_USER_EXIT;
	}

	if (password[strlen(password) - 1] == '\n')
		password[strlen(password) - 1] = 0;

	target = malloc(strlen(password) + 1);
	strcpy(target, password);
	free(password);
	return RESULT_SUCCESS;
}

int update_gender(sqlite3* db, char* target)
{
	char* gender = calloc(100, sizeof(char));
	printf("Enter your password (or press Enter to exit): ");
	fgets(gender, 100, stdin);

	if (gender[0] == '\n')
	{
		return RESULT_USER_EXIT;
	}

	if (gender[strlen(gender) - 1] == '\n')
		gender[strlen(gender) - 1] = 0;

	target = malloc(strlen(gender) + 1);
	strcpy(target, gender);
	free(gender);
	return RESULT_SUCCESS;
}

int update_weight(sqlite3* db, int* target)
{
	int weight;
	do
	{
		printf("Enter your weight (enter a non-number to exit): ");
		if (scanf("%d", &weight) == 0)
		{
			return RESULT_USER_EXIT;
		}
		if (weight <= 0)
		{
			printf("Incorrect number. Try again\n");
		}
	} while (weight <= 0);

	
	*target = weight;
	return RESULT_SUCCESS;
}

int update_height(sqlite3* db, int* target)
{
	int height;
	do
	{
		printf("Enter your height (enter a non-number to exit): ");
		if (scanf("%d", &height) == 0)
		{
			return RESULT_USER_EXIT;
		}
		if (height <= 0)
		{
			printf("Incorrect number. Try again\n");
		}
	} while (height <= 0);

	*target = height;
	return RESULT_SUCCESS;
}