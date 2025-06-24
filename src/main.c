#include "G00_asset.h"
#include "G00_config.h"
#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"

struct G00_App {
	struct G00_Config config;
	sqlite3* db;
	unsigned long long ticks;
	struct G00_Video video;
	struct G00_MemoryState memory;
};

enum G00_AppInitResult {
	G00_APP_INIT_RESULT_OK = 0,
	G00_APP_INIT_RESULT_MEMORY_ERROR = -1,
	G00_APP_INIT_RESULT_DATABASE_ERROR = -2,
	G00_APP_INIT_RESULT_FRAMEWORK_ERROR = -3,
	G00_APP_INIT_RESULT_FONT_RENDERER_ERROR = -4,
	G00_APP_INIT_RESULT_VIDEO_ERROR = -5,
};

enum G00_AppInitResult G00_AppInit(struct G00_App* app, int argc, char* argv[]) {
	G00_ConfigRead("default.app.cfg", &app->config);
	G00_ConfigRead("autoexec.app.cfg", &app->config);

	int memory_result = G00_MemoryInit(&app->memory, app->config.memory);
	if (memory_result < 0) {
		return G00_APP_INIT_RESULT_MEMORY_ERROR;
	}

	int sqlite_result = G00_DatabaseInit(&app->db);
	if (sqlite_result < 0) {
		return G00_APP_INIT_RESULT_DATABASE_ERROR;
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return G00_APP_INIT_RESULT_FRAMEWORK_ERROR;
	}

	if (!TTF_Init()) {
		fprintf(stderr, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());
		return G00_APP_INIT_RESULT_FONT_RENDERER_ERROR;
	}

	app->ticks = 0;
	app->video.app = app;
	if (G00_VideoInit(&app->video, app->config.video)) {
		return G00_APP_INIT_RESULT_VIDEO_ERROR;
	}

	return G00_APP_INIT_RESULT_OK;
}

void G00_AppTeardown(struct G00_App* app) {
	G00_VideoTeardown(&app->video);
	TTF_Quit();
	SDL_Quit();
	G00_DatabaseTeardown(&app->db);
	G00_MemoryTeardown(&app->memory);
}

int main(int argc, char* argv[]) {
	struct G00_App app;
	enum G00_AppInitResult app_init_result = G00_AppInit(&app, argc, argv);
	if (app_init_result < 0) {
		return -1;
	}

	G00_AssetGenerateLoadOrder("assets");
	if (G00_AssetLoadFromOrder("assets", &app.memory) < 0) {
		return -1;
	}

	// ---- script section

	unsigned int image_asset_index;
	if (G00_MemoryRetrieveIndex(&app.memory, "texture.png", &image_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve image!\n");
		return -1;
	}

	unsigned int image_sprite_index;
	int sprite_load_result = G00_VideoLoadImageFromMemory(&app.video, app.memory.entries[image_asset_index].len, app.memory.data + app.memory.entries[image_asset_index].offset, &image_sprite_index);
	if (sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	app.video.loaded_sprites[image_sprite_index].rect = (SDL_FRect) {
		.x = (app.config.video.screen_width / 2.f) - (app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->w / 2.f),
		.y = (app.config.video.screen_height / 2.f) - (app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->h / 2.f),
		.w = app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->h,
	};

	unsigned int font_asset_index;
	if (G00_MemoryRetrieveIndex(&app.memory, "font-ui.ttf", &font_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve font!\n");
		return -1;
	}

	unsigned int font_index;
	int font_load_result = G00_VideoLoadFontFromMemory(&app.video, app.memory.entries[font_asset_index].len, app.memory.data + app.memory.entries[font_asset_index].offset, 48.f, &font_index);
	if (font_load_result < 0) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (font_load_result > 0) {
		fprintf(stdout, "Warning: Font loaded abnormally.\n");
	}

	unsigned int text_sprite_index;
	int text_sprite_load_result = G00_VideoLoadText(&app.video, font_index, "Hello, world!", 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}, &text_sprite_index);
	if (text_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (text_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	app.video.loaded_sprites[text_sprite_index].rect = (SDL_FRect) {
		.x = (app.config.video.screen_width / 2.f) - (app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->w / 2.f),
		.y = app.config.video.screen_height - app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->h,
		.w = app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->h,
	};

	bool quit = false;
	SDL_Event e;
	while (quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
		}

		app.ticks = SDL_GetTicks();
		G00_VideoUpdate(&app.video, app.ticks);
	}

	// ---- end of script section

	G00_AppTeardown(&app);

	return 0;
}
