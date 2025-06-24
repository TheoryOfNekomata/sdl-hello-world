#include "G00_database.h"

enum G00_DatabaseInitResult G00_DatabaseInit(sqlite3** db) {
	int sqlite_result = sqlite3_open("database.sqlite", db);
	if (sqlite_result != SQLITE_OK) {
		fprintf(stderr, "sqlite could not initialize! sqlite3_errstr: %s\n", sqlite3_errstr(sqlite_result));
		return G00_DATABASE_INIT_RESULT_COULD_NOT_OPEN_DATABASE;
	}

	FILE* fp = fopen("schema.sql", "r");
	unsigned long file_size = 0;
	if (fp) {
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		rewind(fp);
		char* schema_str = sqlite3_malloc(file_size + 1);
		fread(schema_str, file_size, file_size, fp);
		fclose(fp);

#ifndef _DEBUG
		unlink("schema.sql");
#endif

		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(*db, schema_str, file_size + 1, &stmt, NULL);
		if (stmt != NULL) {
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
	}

	return G00_DATABASE_INIT_RESULT_OK;
}

enum G00_DatabaseTeardownResult G00_DatabaseTeardown(sqlite3** db) {
	int sqlite_result = sqlite3_close(*db);
	if (sqlite_result != SQLITE_OK) {
		fprintf(stderr, "sqlite could not shut down! sqlite3_errstr: %s\n", sqlite3_errstr(sqlite_result));
		return G00_DATABASE_TEARDOWN_BUSY;
	}

	*db = NULL;
	return G00_DATABASE_TEARDOWN_OK;
}
