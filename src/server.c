#include "../include/interface.h"
#include <stdio.h>

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

	int weight = NULL;
	if (update_weight(db, &weight) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	int height = NULL;
	if (update_height(db, &height) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	char* gender = NULL;
	if (update_gender(db, gender) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}


	sprintf(sql_query, "INSERT INTO Client VALUES ('%s', '%s', '%d', '%d', '%s');",
		login, password, weight, height, gender);

	rc = sqlite3_exec(db, sql_query, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("Failed to add a user: %s\n", err_msg);
		sqlite3_free(err_msg);
		return RESULT_ERROR_UNKNOWN;
	}

	return RESULT_SUCCESS;
}

int authorization(sqlite3* db)
{
	char* err_msg = 0;
	int rc;
	char sql_query[200];

	char login[100];
	char password[100];
	size_t bufsize = 0;
	int client_id;

	do
	{
		printf("Enter your login (or press Enter to exit): ");
		getline(&login, &bufsize, stdin);
		if (strlen(login) == 0 || login[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		printf("Enter your password (or press Enter to exit): ");
		getline(&password, &bufsize, stdin);
		if (strlen(password) == 0 || password[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (login == admin_login && password == admin_password)
		{
			return 0;
		}

		sprintf(sql_query, "SELECT login FROM Client WHERE login = '%s' AND password = '%s';", login, password);
		rc = sqlite3_exec(db, sql_query, callback_auth, &client_id, &err_msg);
		if (rc != SQLITE_OK) {
			printf("Incorrect login or password.\n");
		}
	} while (rc != SQLITE_OK);

	if (err_msg != NULL)
	{
		sqlite3_free(err_msg);
	}

	return client_id;
}

int disp_client(sqlite3* db, int id)
{
	char* query = sqlite3_mprintf("SELECT login, gender, weight, height, plan_id FROM users WHERE id = %d", id);

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error in preparing the request: %s\n", sqlite3_errmsg(db));
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		printf("Hello, %s\n", sqlite3_column_text(stmt, 0));
		printf("Gender: %s\n", sqlite3_column_text(stmt, 1));
		printf("Current weight: %s\n", sqlite3_column_int(stmt, 2));
		printf("Current height: %s\n", sqlite3_column_int(stmt, 3));
		if (sqlite3_column_int(stmt, 4) == NULL)
		{
			printf("You don't have a meal plan yet. Make an order to get daily nutrition recommendations!\n");
		}
		else
		{
			sqlite3_finalize(stmt);
			sqlite3_free(query);
			query = sqlite3_mprintf("SELECT p.type, p.period, m.breakfast, m.lunch, m.dinner, m.calories, "
				"m.proteins, m.fats, m.carbs FROM Plan p INNER JOIN Menu m ON p.menu_id = m.id WHERE p.id = %d", 
				sqlite3_column_int(stmt, 4));
			int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc != SQLITE_OK) {
				fprintf(stderr, "Error in preparing the request: %s\n", sqlite3_errmsg(db));
				return RESULT_ERROR_UNKNOWN;
			}
			rc = sqlite3_step(stmt);
			if (rc == SQLITE_ROW)
			{
				printf("Current plan: %s %d mon.\n", sqlite3_column_text(stmt, 0), sqlite3_column_int(stmt, 1));
				printf("Today's menu:\n");
				printf("Breakfast - %s\n", sqlite3_column_text(stmt, 2));
				printf("Lunch - %s\n", sqlite3_column_text(stmt, 3));
				printf("Dinner - %s\n", sqlite3_column_text(stmt, 4));
				printf("%d cal., %d pr., %d fat., %d carb.\n", 
					sqlite3_column_text(stmt, 5), sqlite3_column_text(stmt, 6), sqlite3_column_text(stmt, 7));
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
	switch (what_to_update)
	{
	case (1):
		if (update_login(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET login = '%s' WHERE id = %d", (char*)target, id);
		break;
	case (2):
		if (update_password(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET password = '%s' WHERE id = %d", (char*)target, id);
		break;
	case (3):
		if (update_weight(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET weight = '%s' WHERE id = %d", *(int*)target, id);
		break;
	case (4):
		if (update_height(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET height = '%s' WHERE id = %d", *(int*)target, id);
		break;
	case (5):
		if (update_gender(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET gender = '%s' WHERE id = %d", (char*)target, id);
		break;
	default:
		printf("Option not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

	char* err_msg = NULL;
	int rc = sqlite3_exec(db, query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when updating data: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int update_login(sqlite3* db, char* target)
{
	char* err_msg = NULL;
	int rc;
	char sql_query[200];
	char login[100];
	size_t bufsize = 0;
	do
	{
		printf("Enter your login (or press Enter to exit): ");
		getline(&login, &bufsize, stdin);
		if (strlen(login) == 0 || login[0] == '\n')
		{
			return RESULT_USER_EXIT;
		}

		if (login == admin_login)
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
	strcpy(target, login);
	return RESULT_SUCCESS;
}

int update_password(sqlite3* db, char* target)
{
	char password[100];
	size_t bufsize = 0;
	printf("Enter your password (or press Enter to exit): ");
	getline(&password, &bufsize, stdin);
	if (strlen(password) == 0 || password[0] == '\n')
	{
		return RESULT_USER_EXIT;
	}
	strcpy(target, password);
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

int update_gender(sqlite3* db, char* target)
{
	char gender[100];
	size_t bufsize = 0;
	printf("Enter your password (or press Enter to exit): ");
	getline(&gender, &bufsize, stdin);
	if (strlen(gender) == 0 || gender[0] == '\n')
	{
		return RESULT_USER_EXIT;
	}
	strcpy(target, gender);
	return RESULT_SUCCESS;
}

int callback_auth(void* client_id, int argc, char** argv, char** column_name) {
	if (argc > 0) {
		*((int*)client_id) = atoi(argv[0]);
	}
	return 0;
}