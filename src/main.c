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

int G00_AppApplyConfig(struct G00_App* app) {
	app->video.config.aspect_ratio = app->config.video.aspect_ratio;
	app->video.config.frames_per_second = app->config.video.frames_per_second;
	app->video.config.millis_per_tick = app->config.video.millis_per_tick;
	app->video.config.screen_width = app->config.video.screen_width;
	app->video.config.screen_height = app->config.video.screen_height;
	app->video.config.max_loaded_fonts = app->config.video.max_loaded_fonts;
	app->video.config.max_loaded_sprites = app->config.video.max_loaded_sprites;
	app->video.config.max_loaded_textures = app->config.video.max_loaded_textures;
	app->memory.config.pool_max_entries = app->config.memory.pool_max_entries;
	app->memory.config.pool_size_bytes = app->config.memory.pool_size_bytes;
	return 0;
}

enum G00_AppInitResult G00_AppInit(struct G00_App* app, int argc, char* argv[]) {
	app->config.app = app;
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

	const float half_screen_x = app.config.video.screen_width / 2.f;
	const float half_screen_y = app.config.video.screen_height / 2.f;

	unsigned int bg_image_asset_index;
	if (G00_MemoryRetrieveIndex(&app.memory, "menu-bg-parallax.png", &bg_image_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve image!\n");
		return -1;
	}

	unsigned int bg_image_sprite_index;
	int bg_sprite_load_result = G00_VideoLoadImageFromMemory(&app.video, app.memory.entries[bg_image_asset_index].len, app.memory.data + app.memory.entries[bg_image_asset_index].offset, &bg_image_sprite_index);
	if (bg_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (bg_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	float base_bg_x = half_screen_x - (app.video.loaded_textures[app.video.loaded_sprites[bg_image_sprite_index].index]->w / 2.f);
	float base_bg_y = half_screen_y - (app.video.loaded_textures[app.video.loaded_sprites[bg_image_sprite_index].index]->h / 2.f);

	app.video.loaded_sprites[bg_image_sprite_index].rect = (SDL_FRect) {
		.x = base_bg_x,
		.y = base_bg_y,
		.w = app.video.loaded_textures[app.video.loaded_sprites[bg_image_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[bg_image_sprite_index].index]->h,
	};

	unsigned int fg_image_asset_index;
	if (G00_MemoryRetrieveIndex(&app.memory, "menu-fg-parallax.png", &fg_image_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve image!\n");
		return -1;
	}

	unsigned int fg_image_sprite_index;
	int fg_sprite_load_result = G00_VideoLoadImageFromMemory(&app.video, app.memory.entries[fg_image_asset_index].len, app.memory.data + app.memory.entries[fg_image_asset_index].offset, &fg_image_sprite_index);
	if (fg_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (fg_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	float base_fg_x = half_screen_x - (app.video.loaded_textures[app.video.loaded_sprites[fg_image_sprite_index].index]->w / 2.f);
	float base_fg_y = half_screen_y - (app.video.loaded_textures[app.video.loaded_sprites[fg_image_sprite_index].index]->h / 2.f);

	app.video.loaded_sprites[fg_image_sprite_index].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app.video.loaded_textures[app.video.loaded_sprites[fg_image_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[fg_image_sprite_index].index]->h,
	};

	unsigned int font_asset_index;
	if (G00_MemoryRetrieveIndex(&app.memory, "font-ui.ttf", &font_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve font!\n");
		return -1;
	}

	unsigned int font_index;
	int font_load_result = G00_VideoLoadFontFromMemory(&app.video, app.memory.entries[font_asset_index].len, app.memory.data + app.memory.entries[font_asset_index].offset, 16.f, &font_index);
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
		.x = half_screen_x - (app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->w / 2.f),
		.y = app.config.video.screen_height - app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->h,
		.w = app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[text_sprite_index].index]->h,
	};

	bool quit = false;
	SDL_Event e;

	const float parallax_offset = 16.f;
	while (quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}

			if (e.type == SDL_EVENT_MOUSE_MOTION) {
				app.video.loaded_sprites[bg_image_sprite_index].rect.x = base_bg_x + ((e.motion.x - half_screen_x) / half_screen_x * -parallax_offset);
				app.video.loaded_sprites[bg_image_sprite_index].rect.y = base_bg_y + ((e.motion.y - half_screen_y) / half_screen_y * -parallax_offset);

				app.video.loaded_sprites[fg_image_sprite_index].rect.x = base_fg_x + ((e.motion.x - half_screen_x) / half_screen_x * parallax_offset);
				app.video.loaded_sprites[fg_image_sprite_index].rect.y = base_fg_y + ((e.motion.y - half_screen_y) / half_screen_y * parallax_offset);
			}
		}

		app.ticks = SDL_GetTicks();
		G00_VideoUpdate(&app.video, app.ticks);
	}

	// ---- end of script section

	G00_AppTeardown(&app);

	return 0;
}
