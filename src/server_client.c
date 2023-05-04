#define _CRT_SECURE_NO_WARNINGS
#include "../include/interface.h"

int callback_client_id(void* client_id, int argc, char** argv, char** column_name) {
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(column_name[i], "id") == 0)
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
		if (strcmp(column_name[i], "height") == 0)
		{
			*((float*)height) = atof(argv[i]);
			break;
		}
	}
	return 0;
}

int callback_weight(void* weight, int argc, char** argv, char** column_name) {
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(column_name[i], "weight") == 0)
		{
			*((float*)weight) = atof(argv[i]);
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
	if (update_login(db, &login) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	char* password = NULL;
	if (update_password(&password) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	char* gender = NULL;
	if (update_gender(&gender) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	float weight = 0;
	if (update_weight(&weight) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	float height = 0;
	if (update_height(&height) == RESULT_USER_EXIT)
	{
		return RESULT_USER_EXIT;
	}

	float bmi = weight / (height * height);

	sprintf(sql_query, "INSERT INTO Client VALUES (NULL, '%s', '%s', '%s', '%f',"
		" '%f', NULL, NULL, '%f');", login, password, gender, weight, height, bmi);

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

	char* login = (char*) malloc(100 * sizeof(char));
	char* password = (char*) malloc(100 * sizeof(char));
	int client_id = -1;

	do
	{
		memset(login, 0, 100);
		memset(password, 0, 100);

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

		sprintf(sql_query, "SELECT id FROM Client WHERE login = '%s' AND password = '%s';", login, password);
		rc = sqlite3_exec(db, sql_query, callback_client_id, &client_id, &err_msg);
		if (rc != SQLITE_OK) {
			printf("Error when trying to log in to the account: %s\n", err_msg);
		}
		if (client_id == -1) {
			system("cls");
			printf("Incorrect login or password.\n");
			printf("---------------------------------\n");
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
		printf("Current weight: %.2f kg\n", sqlite3_column_double(stmt, 2));
		printf("Current height: %.2f m\n", sqlite3_column_double(stmt, 3));
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

			query = sqlite3_mprintf("SELECT type, period FROM Meal_Plan WHERE id = %d", plan_id);		
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

			query = sqlite3_mprintf("SELECT * FROM Meal_Plan WHERE id=? AND ? BETWEEN min_bmi AND max_bmi");
			rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
			if (rc == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, plan_id);
				sqlite3_bind_double(stmt, 2, bmi);
			}
			else {
				printf("Error when trying to display meal plan recommendations: %s\n", sqlite3_errmsg(db));
				return RESULT_ERROR_UNKNOWN;
			}

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW)
			{
				printf("*It is recommended that you change your meal plan\n");
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

int disp_client_menu(sqlite3* db, int client_id)
{
	char* query = sqlite3_mprintf("SELECT m.breakfast, m.lunch, m.dinner, m.calories, "
				"m.proteins, m.fats, m.carbs from Client c JOIN Menu m ON c.menu_id = m.id WHERE c.id = %i", client_id);
				
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display menu information: %s\n", sqlite3_errmsg(db));
		return RESULT_ERROR_UNKNOWN;
	}
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		printf("Today's menu:\n");
		printf("---------------------------------\n");
		printf("Breakfast - %s\n", sqlite3_column_text(stmt, 0));
		printf("Lunch - %s\n", sqlite3_column_text(stmt, 1));
		printf("Dinner - %s\n", sqlite3_column_text(stmt, 2));
		printf("%.3f cal., %.3f pr., %.3f fat., %.3f carb.\n",
				sqlite3_column_double(stmt, 3), sqlite3_column_double(stmt, 4), 
				sqlite3_column_double(stmt, 5), sqlite3_column_double(stmt, 6));
		printf("---------------------------------\n");
	}
	else
	{
		printf("No info... Update your menu first\n");
	}

	sqlite3_free(query);
	sqlite3_finalize(stmt);

	return RESULT_SUCCESS;
}

int update_client(sqlite3* db, int id, int what_to_update)
{
	char* target_str = NULL;
	float target_float;
	int target_int;
	char* query;
	char* err_msg = NULL;
	int rc;
	switch (what_to_update)
	{
	case (1):
		if (update_plan(db, &target_int, id) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET plan_id = %d, menu_id = NULL WHERE id = %d", target_int, id);
		break;
	case (2):
		rc = update_menu(db, &target_int, id);
		if (rc != RESULT_SUCCESS)
		{
			return rc;
		}
		query = sqlite3_mprintf("UPDATE Client SET menu_id = '%d' WHERE id = %d", target_int, id);
		break;
	case (3):
		if (update_weight(&target_float) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		float height = -1;
		query = sqlite3_mprintf("SELECT height FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_height, &height, &err_msg);
		if (rc != SQLITE_OK || height == -1) {
			printf("Error when updating weight: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET weight = '%f', bmi = '%f' WHERE id = %d",
			target_float, target_float / (height * height), id);
		break;
	case (4):
		if (update_height(&target_float) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		float weight = -1;
		query = sqlite3_mprintf("SELECT weight FROM Client WHERE id = %d", id);
		rc = sqlite3_exec(db, query, callback_weight, &weight, &err_msg);
		if (rc != SQLITE_OK || weight == -1) {
			printf("Error when updating height: %s\n", err_msg);
			sqlite3_free(err_msg);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}
		sqlite3_free(query);
		query = sqlite3_mprintf("UPDATE Client SET height = '%f', bmi = '%f' WHERE id = %d",
			target_float, weight / (target_float * target_float), id);
		break;
	case (5):
		if (update_gender(&target_str) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET gender = '%s' WHERE id = %d", target_str, id);
		free(target_str);
		break;
	case (6):
		if (update_login(db, &target_str) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET login = '%s' WHERE id = %d", target_str, id);
		free(target_str);
		break;
	case (7):
		if (update_password(&target_str) == RESULT_USER_EXIT)
		{
			return RESULT_USER_EXIT;
		}
		query = sqlite3_mprintf("UPDATE Client SET password = '%s' WHERE id = %d", target_str, id);
		free(target_str);
		break;
	default:
		system("cls");
		printf("Option not found.\n");
		printf("Press enter to continue...\n");
        getchar();
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

int update_plan(sqlite3* db, int* target_plan_id, int client_id)
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

	query = sqlite3_mprintf("SELECT DISTINCT type FROM Meal_Plan WHERE "
	"(min_bmi <= %f OR min_bmi = NULL) AND (max_bmi > %f OR max_bmi = NULL)", bmi, bmi);
	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display the proposed plans: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	sqlite3_step(stmt);
	sqlite3_free(query);

	int amount_plan_types = 3;
	char* plan_types[] = {"low-calorie","moderate","high-calorie"}; 
	int usr_input;

	do
	{
		printf("---------------------------------\n");
        for(int i = 0; i < amount_plan_types; i++)
		{
			printf("%d - %s", i+1, plan_types[i]);
			if (strcmp(plan_types[i],sqlite3_column_text(stmt,0)) == 0)
			{
				printf("\t<- *Recommended*");
			}
			printf("\n");
		}
        printf("---------------------------------\n");
        printf("Enter the number of the option (enter a non-number to exit): ");

		if (scanf("%d", &usr_input) == 0)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF) {};
			sqlite3_finalize(stmt);
			return RESULT_USER_EXIT;
		}
		
		system("cls");
		if (usr_input < 0 || usr_input > amount_plan_types)
		{
			printf("Incorrect number. Try again\n");
		}
		else
		{
			query = sqlite3_mprintf("SELECT id, period, price FROM Meal_Plan WHERE type = '%s' AND id != %d", 
				plan_types[usr_input-1], plan_cur_id);
		}
	}while(usr_input < 0 || usr_input > amount_plan_types);

	sqlite3_finalize(stmt);
	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error when trying to display the proposed plans: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_free(query);
		return RESULT_ERROR_UNKNOWN;
	}

	int amount_periods_max = 3;
	int* plans_id = (int*)calloc(amount_periods_max, sizeof(int));

	int found = 0;
	do
	{
		printf("---------------------------------\n");
		printf("ID\tPeriod(mon.)\tPrice\n");

		for (int i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++) {
			plans_id[i] = sqlite3_column_int(stmt, 0);
			printf("%d\t%d\t$%.2f\n", plans_id[i], sqlite3_column_int(stmt, 1),
				sqlite3_column_double(stmt, 2));
		}

		printf("---------------------------------\n");
		printf("Enter the ID of the meal plan you want to order (enter a non-number to exit): ");
		if (scanf("%d", target_plan_id) == 0)
		{
			return RESULT_USER_EXIT;
		}
		system("cls");
		for (int i = 0; i < amount_periods_max && plans_id[i] != 0; i++)
		{
			if (*target_plan_id == plans_id[i])
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

	query = sqlite3_mprintf("INSERT INTO Orders VALUES(NULL, '%d', '%s', '%d'); ",
		client_id, datestr, *target_plan_id);
	rc = sqlite3_exec(db, query, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		printf("Failed to process an order: %s\n", err_msg);
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
		if (plan_id == 0)
		{
			printf("Can't choose a menu: You don't have a meal plan yet\n");
			printf("Press enter to continue...\n");
        	getchar();
			sqlite3_free(query);
			sqlite3_finalize(stmt);
			return RESULT_ERROR_UNKNOWN;
		}
		int menu_id = sqlite3_column_int(stmt, 1);
		sqlite3_free(query);
		sqlite3_finalize(stmt);
		query = sqlite3_mprintf("SELECT m.id, m.breakfast, m.lunch, m.dinner, m.calories, m.proteins, "
			"m.fats, m.carbs FROM Menu m JOIN Meal_Plan p on m.plan_type = p.type WHERE p.id = %d AND m.id != %d", 
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
		
		int found = 0;
		do
		{
			printf("------------------------------------------\n");

			for(int i = 0; sqlite3_step(stmt) == SQLITE_ROW; i++)
			{
				menus_id[i] = sqlite3_column_int(stmt, 0);
				printf("ID - %d\n", menus_id[i]);
				printf("Breakfast - %s\n", sqlite3_column_text(stmt, 1));
				printf("Lunch - %s\n", sqlite3_column_text(stmt, 2));
				printf("Dinner - %s\n", sqlite3_column_text(stmt, 3));
				printf("%.3f cal., %.3f pr., %.3f fat., %.3f carb.\n",
					sqlite3_column_double(stmt, 4), sqlite3_column_double(stmt, 5),
					sqlite3_column_double(stmt, 6), sqlite3_column_double(stmt, 7));
				printf("------------------------------------------\n");
			}
			printf("Enter the ID of the menu you prefer (enter a non-number to exit): ");
			if (scanf("%d", target_menu_id) == 0)
			{
				char c;
				while ((c = getchar()) != '\n' && c != EOF) {};
				return RESULT_USER_EXIT;
			}
			system("cls");
			for (int i = 0; i < max_menus_amount && menus_id[i] != 0; i++)
			{
				if (*target_menu_id == menus_id[i])
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

int update_login(sqlite3* db, char** target)
{
	int rc;
	char* query = NULL;
	char* login = (char*) malloc(100 * sizeof(char));
	sqlite3_stmt* stmt;
	do
	{
		system("cls");
		memset(login, 0, 100);
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
			system("cls");
			printf("A user with this login already exists.\n");
			printf("Press enter to continue...\n");
        	getchar();
			rc = SQLITE_ROW;
			continue;
		}

		query = sqlite3_mprintf("SELECT login FROM Client WHERE login = '%s';", login);
		rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

		if (rc != SQLITE_OK) {
			printf("Error when trying to update login: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_free(query);
			return RESULT_ERROR_UNKNOWN;
		}

		rc = sqlite3_step(stmt);

		if (rc == SQLITE_ROW) {
			system("cls");
			printf("A user with this login already exists.\n");
			printf("Press enter to continue...\n");
        	getchar();
		}

		sqlite3_finalize(stmt);
		sqlite3_free(query);

	} while (rc == SQLITE_ROW);

	*target = login;
	return RESULT_SUCCESS;
}

int update_password(char** target)
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

	*target = password;
	return RESULT_SUCCESS;
}

int update_gender(char** target)
{
	char* gender = calloc(100, sizeof(char));
	printf("Enter your gender (or press Enter to exit): ");
	fgets(gender, 100, stdin);

	if (gender[0] == '\n')
	{
		return RESULT_USER_EXIT;
	}

	if (gender[strlen(gender) - 1] == '\n')
		gender[strlen(gender) - 1] = 0;

	*target = gender;
	return RESULT_SUCCESS;
}

int update_weight(float* target)
{
	do
	{
		printf("Enter your weight in kilograms (enter a non-number to exit): ");
		if (scanf("%f", target) == 0)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF) {};
			return RESULT_USER_EXIT;
		}
		if (*target <= 0)
		{
			system("cls");
			printf("Incorrect number. Try again\n");
			printf("------------------------------------------\n");
		}
	} while (*target <= 0);

	return RESULT_SUCCESS;
}

int update_height(float* target)
{
	do
	{
		printf("Enter your height in meteres (enter a non-number to exit): ");
		if (scanf("%f", target) == 0)
		{
			char c;
			while ((c = getchar()) != '\n' && c != EOF) {};
			return RESULT_USER_EXIT;
		}
		if (*target <= 0)
		{
			system("cls");
			printf("Incorrect number. Try again\n");
			printf("------------------------------------------\n");
		}
	} while (*target <= 0);

	return RESULT_SUCCESS;
}
