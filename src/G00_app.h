#ifndef G00_APP_H
#define G00_APP_H

#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"
#include "G00_ui.h"

enum G00_AppMode : unsigned char {
	G00_APP_MODE_UI,
	G00_APP_MODE_GAME,
};

struct G00_GameState {
	unsigned int menu_bg_asset_index; // memory
	unsigned int menu_fg_asset_index;
	unsigned int primary_font_asset_index;

	unsigned int primary_font_index;

	unsigned int menu_bg_sprite_index;
	unsigned int menu_fg_sprite_index;
	unsigned int menu_fg_shadow0_sprite_index;
	unsigned int menu_fg_shadow1_sprite_index;
	unsigned int menu_fg_shadow2_sprite_index;
	unsigned int menu_fg_shadow3_sprite_index;
};

struct G00_App {
	enum G00_AppMode mode;
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
