#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#include "G00_config.h"
#include "G00_video.h"

struct G00_App {
	struct G00_Config config;
	unsigned long long ticks;
	struct G00_Video video;
};

unsigned long G00_AppGetTicks(struct G00_App* app) {
	return app->ticks;
}

void G00_LoadTexture(SDL_Renderer* renderer, const char* path, SDL_Texture** out0_loaded_surface) {
	SDL_Surface* image = IMG_Load(path);
	if (image == NULL) {
		return;
	}

	*out0_loaded_surface = SDL_CreateTextureFromSurface(renderer, image);
	SDL_DestroySurface(image);
}

void G00_LoadFont(const char* path, float size, TTF_Font** out0_loaded_font) {
	*out0_loaded_font = TTF_OpenFont(path, size);
}

void G00_RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, size_t text_len, SDL_Color color, SDL_Texture** out0_rendered_text_surface) {
	SDL_Surface* rendered = TTF_RenderText_Blended(font, text, text_len, color);
	if (rendered == NULL) {
		return;
	}

	*out0_rendered_text_surface = SDL_CreateTextureFromSurface(renderer, rendered);
	SDL_DestroySurface(rendered);
}

enum G00_AppInitResult {
	G00_APP_INIT_RESULT_OK = 0,
	G00_APP_INIT_RESULT_FRAMEWORK_ERROR = -1,
	G00_APP_INIT_RESULT_FONT_RENDERER_ERROR = -2,
	G00_APP_INIT_RESULT_VIDEO_ERROR = -3,
};

enum G00_AppInitResult G00_AppInit(struct G00_App* app) {
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
}

int main(int argc, char* argv[]) {
	struct G00_App app;
	G00_ConfigRead("default.cfg", &app.config);
	G00_ConfigRead("autoexec.cfg", &app.config);

	enum G00_AppInitResult app_init_result = G00_AppInit(&app);
	if (app_init_result < 0) {
		return -1;
	}

	unsigned int image_sprite_index;
	int sprite_load_result = G00_VideoLoadImageObject(&app.video, "assets/images/image.png", &image_sprite_index);
	if (sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	unsigned int font_index;
	int font_load_result = G00_VideoLoadFont(&app.video, "assets/fonts/open-sans/OpenSans-Regular.ttf", 48.f, &font_index);
	if (font_load_result < 0) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (font_load_result > 0) {
		fprintf(stdout, "Warning: Font loaded abnormally.\n");
	}

	unsigned int text_sprite_index;
	int text_sprite_load_result = G00_VideoLoadTextObject(&app.video, font_index, "Hello, world!", 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}, &text_sprite_index);
	if (text_sprite_load_result < 0) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -1;
	} if (text_sprite_load_result > 0) {
		fprintf(stdout, "Warning: Sprite loaded abnormally.\n");
	}

	app.video.loaded_sprites[image_sprite_index].rect = (SDL_FRect) {
		.x = (app.config.video.screen_width / 2.f) - (app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->w / 2.f),
		.y = (app.config.video.screen_height / 2.f) - (app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->h / 2.f),
		.w = app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->w,
		.h = app.video.loaded_textures[app.video.loaded_sprites[image_sprite_index].index]->h,
	};

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

	G00_AppTeardown(&app);

	return 0;
}
