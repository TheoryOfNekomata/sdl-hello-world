#include "G00_video.h"

enum G00_VideoInitResult G00_VideoInit(struct G00_Video* video, struct G00_VideoConfig config) {
	if (!video) {
		return G00_VIDEO_INIT_RESULT_UNKNOWN_VIDEO_STATE;
	}

	if (!video->app) {
		return G00_VIDEO_INIT_RESULT_UNBOUND_VIDEO_STATE;
	}

	video->config.screen_width = config.screen_width;
	video->config.screen_height = config.screen_height;
	video->config.frames_per_second = config.frames_per_second;
	video->config.millis_per_tick = config.millis_per_tick;

	video->window = SDL_CreateWindow(G00_APP_NAME, video->config.screen_width, video->config.screen_height, 0);
	if (video->window == NULL) {
		return G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_WINDOW;
	}
	video->renderer = SDL_CreateRenderer(video->window, NULL);
	if (video->renderer == NULL) {
		return G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_RENDERER;
	}
	SDL_SetRenderDrawColor(video->renderer, 0x00, 0x00, 0x00, 0xFF);

	memset(video->loaded_fonts, 0, sizeof(video->loaded_fonts));
	memset(video->loaded_textures, 0, sizeof(video->loaded_textures));
	memset(video->loaded_sprites, 0, sizeof(video->loaded_sprites));

	return G00_VIDEO_INIT_RESULT_OK;
}

void G00_VideoUpdate(struct G00_Video* video, unsigned long app_ticks) {
	float ticks_elapsed = (float) (app_ticks - video->ticks);
	if (ticks_elapsed <= video->config.millis_per_tick) {
		return;
	}

	SDL_SetRenderDrawColor(video->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(video->renderer);
	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_SPRITES; i += 1) {
		if (video->loaded_sprites[i].type == G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE) {
			SDL_RenderTexture(
				video->renderer,
				video->loaded_textures[video->loaded_sprites[i].index],
				NULL,
				&video->loaded_sprites[i].rect
			);
		}
	}

#if _DEBUG
	SDL_SetRenderDrawColor(video->renderer, 0x00, 0xFF, 0xFF, 0xFF);
	for (unsigned char i = 0; i < 64; i += 1) {
		if ((app_ticks & (1ull << i)) > 0ull) {
			SDL_RenderFillRect(video->renderer, &(SDL_FRect) {
				.x = video->config.screen_width - (4 * i),
				.y = video->config.screen_height - 4,
				.w = 4.f,
				.h = 4.f
			});
		}
	}
#endif

	SDL_RenderPresent(video->renderer);
	video->ticks = app_ticks;
}

int G00_VideoLoadImageObject(struct G00_Video* video, const char* path, unsigned int* out0_index) {
	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_TEXTURES; i += 1) {
		if (video->loaded_textures[i] != NULL) {
			continue;
		}

		SDL_Surface* image = IMG_Load(path);
		if (image == NULL) {
			return -1;
		}

		video->loaded_textures[i] = SDL_CreateTextureFromSurface(video->renderer, image);
		if (video->loaded_textures[i] == NULL) {
			return -2;
		}
		SDL_DestroySurface(image);

		for (unsigned int j = 0; j < G00_VIDEO_MAX_LOADED_SPRITES; j += 1) {
			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
				continue;
			}

			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
			*out0_index = video->loaded_sprites[j].index = i;
			return 0;
		}
	}

	return 1;
}

int G00_VideoLoadTextObject(struct G00_Video* video, unsigned int font_index, const char* text, size_t text_len, SDL_Color color, unsigned int* out0_texture_index) {
	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_TEXTURES; i += 1) {
		if (video->loaded_textures[i] != NULL) {
			continue;
		}

		SDL_Surface* image = TTF_RenderText_Blended(video->loaded_fonts[font_index], text, text_len, color);
		if (image == NULL) {
			return -1;
		}

		video->loaded_textures[i] = SDL_CreateTextureFromSurface(video->renderer, image);
		if (video->loaded_textures[i] == NULL) {
			return -2;
		}
		SDL_DestroySurface(image);

		for (unsigned int j = 0; j < G00_VIDEO_MAX_LOADED_SPRITES; j += 1) {
			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
				continue;
			}

			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
			*out0_texture_index = video->loaded_sprites[j].index = i;
			return 0;
		}
	}

	return 1;
}

int G00_VideoLoadFont(struct G00_Video* video, const char* path, float size, unsigned int* out0_index) {
	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_FONTS; i += 1) {
		if (video->loaded_fonts[i] != NULL) {
			continue;
		}

		video->loaded_fonts[i] = TTF_OpenFont(path, size);
		if (video->loaded_fonts[i] == NULL) {
			return -1;
		}

		*out0_index = i;
		return 0;
	}

	return 1;
}

void G00_VideoTeardown(struct G00_Video* video) {
	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_TEXTURES; i += 1) {
		if (video->loaded_textures[i] == NULL) {
			continue;
		}
		SDL_DestroyTexture(video->loaded_textures[i]);
		video->loaded_textures[i] = NULL;
	}

	for (unsigned int i = 0; i < G00_VIDEO_MAX_LOADED_FONTS; i += 1) {
		if (video->loaded_fonts[i] == NULL) {
			continue;
		}
		TTF_CloseFont(video->loaded_fonts[i]);
		video->loaded_fonts[i] = NULL;
	}

	SDL_DestroyRenderer(video->renderer);
	video->renderer = NULL;

	SDL_DestroyWindow(video->window);
	video->window = NULL;
}
