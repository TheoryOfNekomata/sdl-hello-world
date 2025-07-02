#ifndef G00_APP_H
#define G00_APP_H

#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"
#include "G00_ui.h"

struct G00_App {
	sqlite3* db;
	unsigned long long ticks;
	struct G00_Video video;
	struct G00_MemoryState memory;
	struct G00_UIState ui;
	struct {
		unsigned char force_exit : 1;
	} flags;
};

#endif
