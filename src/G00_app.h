#ifndef G00_APP_H
#define G00_APP_H

#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"
#include "G00_ui.h"

struct G00_GameState {
	unsigned int font_index;

	unsigned int bg_image_asset_index; // memory

	unsigned int bg_image_sprite_index;
	unsigned int fg_image_sprite_index;
	unsigned int fg_image_sprite_index_shadow0;
	unsigned int fg_image_sprite_index_shadow1;
	unsigned int fg_image_sprite_index_shadow2;
	unsigned int fg_image_sprite_index_shadow3;
};

struct G00_App {
	sqlite3* db;
	unsigned long long ticks;
	struct G00_Video video;
	struct G00_MemoryState memory;
	struct G00_UIState ui;
	struct G00_GameState game_state;
	struct {
		unsigned char force_exit : 1;
	} flags;
};

#endif
