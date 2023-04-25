#include "../include/interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int disp_all_clients(sqlite3* db)
{
	const char* sql = "SELECT * FROM Client;";
	if (sqlite3_exec(db, sql, callback, 0, 0) != SQLITE_OK) {
		return RESULT_ERROR_UNKNOWN;
	}
}

static int callback(void* data, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("-------------------------------------\n");
	return 0;
}