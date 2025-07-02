#ifndef G00_DATABASE_H
#define G00_DATABASE_H

#include <sqlite/sqlite3.h>

enum G00_DatabaseInitResult {
	G00_DATABASE_INIT_RESULT_OK = 0,
	G00_DATABASE_INIT_RESULT_COULD_NOT_OPEN_DATABASE = -1,
};

enum G00_DatabaseInitResult G00_DatabaseInit(sqlite3**);

enum G00_DatabaseTeardownResult {
	G00_DATABASE_TEARDOWN_OK = 0,
	G00_DATABASE_TEARDOWN_BUSY = -1,
};

enum G00_DatabaseTeardownResult G00_DatabaseTeardown(sqlite3**);

#endif
