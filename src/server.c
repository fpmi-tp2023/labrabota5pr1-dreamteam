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