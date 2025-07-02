#include "G00_asset.h"
#include "G00_config.h"

enum G00_AppInitResult {
	G00_APP_INIT_RESULT_OK = 0,
	G00_APP_INIT_RESULT_MEMORY_ERROR = -1,
	G00_APP_INIT_RESULT_DATABASE_ERROR = -2,
	G00_APP_INIT_RESULT_FRAMEWORK_ERROR = -3,
	G00_APP_INIT_RESULT_FONT_RENDERER_ERROR = -4,
	G00_APP_INIT_RESULT_VIDEO_ERROR = -5,
};

enum G00_AppInitResult G00_AppInit(struct G00_App* app, int argc, char* argv[]) {
	bool load_custom_configs = true;
	for (unsigned int i = 1; i < argc; i += 1) {
		if (!strcmp("--ignore-custom", argv[i])) {
			load_custom_configs = false;
		}
	}

	G00_ConfigExecuteScript("default.app.cfg", app);
	if (load_custom_configs) {
		G00_ConfigExecuteScript("autoexec.app.cfg", app);
	}

	int memory_result = G00_MemoryInit(&app->memory);
	if (memory_result < 0) {
		return G00_APP_INIT_RESULT_MEMORY_ERROR;
	}

	int sqlite_result = G00_DatabaseInit(&app->db);
	if (sqlite_result < 0) {
		return G00_APP_INIT_RESULT_DATABASE_ERROR;
	}

	G00_ConfigExecuteScript("default.asset.cfg", app);
	if (load_custom_configs) {
		G00_ConfigExecuteScript("autoexec.asset.cfg", app);
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
	if (G00_VideoInit(&app->video)) {
		return G00_APP_INIT_RESULT_VIDEO_ERROR;
	}

	G00_ConfigExecuteScript("default.menu.cfg", app);
	return G00_APP_INIT_RESULT_OK;
}

int G00_AppUpdate(struct G00_App* app) {
	const float half_screen_x = app->video.config.screen_width / 2.f;
	const float half_screen_y = app->video.config.screen_height / 2.f;

	unsigned int font_asset_index;
	if (G00_MemoryRetrieveIndex(&app->memory, "font-ui.ttf", &font_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve font!\n");
		return -1;
	}

	unsigned int font_index;
	int font_load_result = G00_VideoLoadFont(&app->video, app->memory.entries[font_asset_index].len, app->memory.data + app->memory.entries[font_asset_index].offset, 16.f, &font_index);
	if (font_load_result < 0) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (font_load_result > 0) {
		fprintf(stdout, "Warning: Font loaded abnormally.\n");
	}

	unsigned int text_sprite_index;
	int text_sprite_load_result = G00_VideoGenerateTextSprite(&app->video, font_index, "Hello, world!", 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}, &text_sprite_index);
	if (text_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (text_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	app->video.loaded_sprites[text_sprite_index].rect = (SDL_FRect) {
		.x = half_screen_x - (app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->w / 2.f),
		.y = app->video.config.screen_height - app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->h,
		.w = app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[text_sprite_index].texture_index]->h,
	};

	unsigned int fg_image_asset_index;
	if (G00_MemoryRetrieveIndex(&app->memory, "menu-fg-parallax.png", &fg_image_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve image!\n");
		return -1;
	}

	SDL_Surface* fg_surface;
	unsigned int fg_image_sprite_index;
	int fg_sprite_load_result = G00_VideoLoadImageSprite(
		&app->video,
		app->memory.entries[fg_image_asset_index].len,
		app->memory.data + app->memory.entries[fg_image_asset_index].offset,
		&fg_image_sprite_index,
		&fg_surface
	);
	if (fg_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (fg_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	float base_fg_x = half_screen_x - (app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index].texture_index]->w / 2.f);
	float base_fg_y = half_screen_y - (app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index].texture_index]->h / 2.f);

	unsigned int fg_image_sprite_index_shadow0;
	G00_VideoGenerateSurfaceSprite(&app->video, fg_surface, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0x00, .a = 0xFF }, &fg_image_sprite_index_shadow0);

	unsigned int fg_image_sprite_index_shadow1;
	G00_VideoGenerateSurfaceSprite(&app->video, fg_surface, (SDL_Color) { .r = 0xFF, .g = 0x00, .b = 0xFF, .a = 0xFF }, &fg_image_sprite_index_shadow1);

	unsigned int fg_image_sprite_index_shadow2;
	G00_VideoGenerateSurfaceSprite(&app->video, fg_surface, (SDL_Color) { .r = 0x00, .g = 0xFF, .b = 0xFF, .a = 0xFF }, &fg_image_sprite_index_shadow2);

	unsigned int fg_image_sprite_index_shadow3;
	G00_VideoGenerateSurfaceSprite(&app->video, fg_surface, (SDL_Color) { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF }, &fg_image_sprite_index_shadow3);
	SDL_DestroySurface(fg_surface);

	app->video.loaded_sprites[fg_image_sprite_index].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index].texture_index]->h,
	};

	app->video.loaded_sprites[fg_image_sprite_index_shadow0].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow0].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow0].texture_index]->h,
	};

	app->video.loaded_sprites[fg_image_sprite_index_shadow1].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow1].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow1].texture_index]->h,
	};

	app->video.loaded_sprites[fg_image_sprite_index_shadow2].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow2].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow2].texture_index]->h,
	};

	app->video.loaded_sprites[fg_image_sprite_index_shadow3].rect = (SDL_FRect) {
		.x = base_fg_x,
		.y = base_fg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow3].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[fg_image_sprite_index_shadow3].texture_index]->h,
	};

	unsigned int bg_image_asset_index;
	if (G00_MemoryRetrieveIndex(&app->memory, "menu-bg-parallax.png", &bg_image_asset_index) < 0) {
		fprintf(stderr, "Unable to retrieve image!\n");
		return -1;
	}

	unsigned int bg_image_sprite_index;
	int bg_sprite_load_result = G00_VideoLoadImageSprite(
		&app->video,
		app->memory.entries[bg_image_asset_index].len,
		app->memory.data + app->memory.entries[bg_image_asset_index].offset,
		&bg_image_sprite_index,
		NULL
	);
	if (bg_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (bg_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	float base_bg_x = half_screen_x - (app->video.loaded_textures[app->video.loaded_sprites[bg_image_sprite_index].texture_index]->w / 2.f);
	float base_bg_y = half_screen_y - (app->video.loaded_textures[app->video.loaded_sprites[bg_image_sprite_index].texture_index]->h / 2.f);

	app->video.loaded_sprites[bg_image_sprite_index].rect = (SDL_FRect) {
		.x = base_bg_x,
		.y = base_bg_y,
		.w = app->video.loaded_textures[app->video.loaded_sprites[bg_image_sprite_index].texture_index]->w,
		.h = app->video.loaded_textures[app->video.loaded_sprites[bg_image_sprite_index].texture_index]->h,
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
				app->video.loaded_sprites[bg_image_sprite_index].rect.x = base_bg_x + ((e.motion.x - half_screen_x) / half_screen_x * -parallax_offset);
				app->video.loaded_sprites[bg_image_sprite_index].rect.y = base_bg_y + ((e.motion.y - half_screen_y) / half_screen_y * -parallax_offset);

				app->video.loaded_sprites[fg_image_sprite_index].rect.x = base_fg_x + ((e.motion.x - half_screen_x) / half_screen_x * parallax_offset);
				app->video.loaded_sprites[fg_image_sprite_index].rect.y = base_fg_y + ((e.motion.y - half_screen_y) / half_screen_y * parallax_offset);

				app->video.loaded_sprites[fg_image_sprite_index_shadow0].rect.x = base_fg_x + 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 32.f));
				app->video.loaded_sprites[fg_image_sprite_index_shadow0].rect.y = base_fg_y + 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 64.f));

				app->video.loaded_sprites[fg_image_sprite_index_shadow1].rect.x = base_fg_x - 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 64.f));
				app->video.loaded_sprites[fg_image_sprite_index_shadow1].rect.y = base_fg_y - 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 32.f));

				app->video.loaded_sprites[fg_image_sprite_index_shadow2].rect.x = base_fg_x + 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 32.f));
				app->video.loaded_sprites[fg_image_sprite_index_shadow2].rect.y = base_fg_y - 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 32.f));

				app->video.loaded_sprites[fg_image_sprite_index_shadow3].rect.x = base_fg_x - 32.f + ((e.motion.x - half_screen_x) / half_screen_x * (parallax_offset + 64.f));
				app->video.loaded_sprites[fg_image_sprite_index_shadow3].rect.y = base_fg_y + 32.f + ((e.motion.y - half_screen_y) / half_screen_y * (parallax_offset + 64.f));
			}
		}

		app->ticks = SDL_GetTicks();
		G00_VideoUpdate(&app->video, app->ticks);
	}

	return 0;
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

	int app_update_result = G00_AppUpdate(&app);
	if (app_update_result < 0) {
		return app_update_result;
	}

	G00_AppTeardown(&app);

	return 0;
}
