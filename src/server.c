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

	char* gender = NULL;
	if (update_gender(db, gender) == RESULT_USER_EXIT)
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

	float bmi = (float)weight / (height * height);

	sprintf(sql_query, "INSERT INTO Client VALUES ('%s', '%s', '%s', '%d', '%d', '%f');",
		login, password, gender, weight, height, bmi);

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
	int client_id = -1;

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
		rc = sqlite3_exec(db, sql_query, callback_client_id, &client_id, &err_msg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "error when trying to log in to the account: %s\n", err_msg);
		}
		if (client_id == -1) {
			printf("Incorrect login or password.\n");
		}
	} while (rc != SQLITE_OK || client_id == -1);

	if (err_msg != NULL)
	{
		sqlite3_free(err_msg);
	}

	return client_id;
}

int disp_client(sqlite3* db, int id)
{
	char* query = sqlite3_mprintf("SELECT login, gender, weight, height, bmi, plan_id FROM users WHERE id = %d", id);

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when trying to display information: %s\n", sqlite3_errmsg(db));
		return RESULT_ERROR_UNKNOWN;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		printf("Hello, %s\n", sqlite3_column_text(stmt, 0));
		printf("Gender: %s\n", sqlite3_column_text(stmt, 1));
		printf("Current weight: %s\n", sqlite3_column_int(stmt, 2));
		printf("Current height: %s\n", sqlite3_column_int(stmt, 3));
		printf("Current BMI: %.3f\n", sqlite3_column_int(stmt, 4));
		if (sqlite3_column_int(stmt, 5) == NULL)
		{
			printf("No meal plan yet.\n");
		}
		else
		{
			sqlite3_free(query);
			query = sqlite3_mprintf("SELECT p.type, p.period, m.breakfast, m.lunch, m.dinner, m.calories, "
				"m.proteins, m.fats, m.carbs FROM Plan p INNER JOIN Menu m ON p.menu_id = m.id WHERE p.id = %d", 
				sqlite3_column_int(stmt, 5));
			sqlite3_finalize(stmt);
			int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc != SQLITE_OK) {
				fprintf(stderr, "Error when trying to display meal plan information: %s\n", sqlite3_errmsg(db));
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
	char* err_msg = NULL;
	int rc;
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
		if (update_gender(db, (char*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET gender = '%s' WHERE id = %d", (char*)target, id);
		break;
	case (4):
		if (update_weight(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		int height = -1;
		query = sqlite3_mprintf("SELECT height FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_height, &height, &err_msg);
		if (rc != SQLITE_OK || height == -1) {
			fprintf(stderr, "Error when updating weight: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET weight = '%s', bmi = '%f' WHERE id = %d", 
			*((int*)target), (float)*((int*)target) / (height * height), id);
		break;
	case (5):
		if (update_height(db, (int*)target) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		int weight = -1;
		query = sqlite3_mprintf("SELECT weight FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_weight, &weight, &err_msg);
		if (rc != SQLITE_OK || weight == -1) {
			fprintf(stderr, "Error when updating height: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET height = '%s', bmi = '%f' WHERE id = %d", 
			*((int*)target), weight / ((*((int*)target)) * (*((int*)target)), id));
		break;
	default:
		printf("Option not found.\n");
		return RESULT_ERROR_UNKNOWN;
	}

	
    rc = sqlite3_exec(db, query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error when updating data: %s\n", err_msg);
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
		fprintf(stderr, "Error when deleting an account: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_free(query);
	return RESULT_SUCCESS;
}

int make_order(sqlite3* db, int id)
{

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