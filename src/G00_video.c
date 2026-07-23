#include <SDL3_image/SDL_image.h>

#include "G00_video.h"

enum G00_VideoInitResult G00_VideoInit(struct G00_Video* video) {
	if (!video) {
		return G00_VIDEO_INIT_RESULT_UNKNOWN_VIDEO_STATE;
	}

	if (!video->app) {
		return G00_VIDEO_INIT_RESULT_UNBOUND_VIDEO_STATE;
	}

	video->window = SDL_CreateWindow(G00_APP_NAME, video->config.screen_width, video->config.screen_height, 0);
	if (video->window == NULL) {
		return G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_WINDOW;
	}
	video->renderer = SDL_CreateRenderer(video->window, NULL);
	if (video->renderer == NULL) {
		return G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_RENDERER;
	}
	SDL_SetRenderDrawColor(video->renderer, 0x00, 0x00, 0x00, 0xFF);

	size_t loaded_fonts_len = sizeof(TTF_Font*) * video->config.max_loaded_fonts;
	video->loaded_fonts = malloc(loaded_fonts_len);
	memset(video->loaded_fonts, 0, loaded_fonts_len);
	size_t loaded_surfaces_len = sizeof(SDL_Surface*) * video->config.max_loaded_surfaces;
	video->loaded_surfaces = malloc(loaded_surfaces_len);
	memset(video->loaded_surfaces, 0, loaded_surfaces_len);
	size_t loaded_sprites_len = sizeof(struct G00_VideoSprite) * video->config.max_loaded_sprites;
	video->loaded_sprites = malloc(loaded_sprites_len);
	memset(video->loaded_sprites, 0, loaded_sprites_len);

	return G00_VIDEO_INIT_RESULT_OK;
}

int G00_VideoUpdateWindow(struct G00_Video* video) {
	if (SDL_SetWindowSize(video->window, video->config.screen_width, video->config.screen_height)) {
		return 0;
	}

	return 1;
}

void G00_VideoUpdate(struct G00_Video* video, unsigned long app_ticks) {
	float ticks_elapsed = (float) (app_ticks - video->ticks);
	if (ticks_elapsed <= video->config.millis_per_tick) {
		return;
	}

	SDL_SetRenderDrawColor(video->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(video->renderer);
	for (unsigned int i = video->config.max_loaded_sprites - 1; i > 0; i -= 1) {
	// for (unsigned int i = 1; i <= video->config.max_loaded_sprites - 1; i += 1) {
		if (video->loaded_sprites[i].type == G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE) {
			SDL_Surface* surface = video->loaded_surfaces[video->loaded_sprites[i].surface_index];
			SDL_Texture* texture = SDL_CreateTextureFromSurface(video->renderer, surface);
			if (texture != NULL) {
				SDL_RenderTexture(
					video->renderer,
					texture,
					NULL,
					&video->loaded_sprites[i].rect
				);
				SDL_DestroyTexture(texture);
			}
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

int G00_VideoLoadImageSprite(struct G00_Video* video, size_t len, void* mem, unsigned int* out0_index) {
	for (unsigned int i = 0; i < video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] != NULL) {
			continue;
		}

		SDL_Surface* image = IMG_Load_IO(SDL_IOFromMem(mem, len), true);
		if (image == NULL) {
			return -1;
		}

		video->loaded_surfaces[i] = image;
		for (unsigned int j = 0; j < video->config.max_loaded_sprites; j += 1) {
			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
				continue;
			}

			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
			video->loaded_sprites[j].surface_index = i;
			*out0_index = j;
			return 0;
		}
	}

	return 1;
}

void G00_VideoSurfaceSetNonAlphaPixelColor(SDL_Surface* copy, SDL_Color color) {
	SDL_LockSurface(copy);
	unsigned char* target_pixel_bytes = copy->pixels;
	unsigned char bytes_per_pixel = SDL_BYTESPERPIXEL(copy->format);

	unsigned char alpha;
	for (unsigned int k = 0; k < copy->h * copy->w * bytes_per_pixel; k += bytes_per_pixel) {
		unsigned int x = k / bytes_per_pixel % copy->w;
		unsigned int y = k / bytes_per_pixel / copy->w;
		SDL_ReadSurfacePixel(copy, x, y, NULL, NULL, NULL, &alpha);
		unsigned int pixel_data = SDL_MapSurfaceRGBA(copy, color.r, color.g, color.b, alpha);
		target_pixel_bytes[k] = pixel_data & 0x000000FFu;
		if (bytes_per_pixel > 1) {
			target_pixel_bytes[k + 1] = (pixel_data & 0x0000FF00u) >> 0x8;
			if (bytes_per_pixel > 2) {
				target_pixel_bytes[k + 2] = (pixel_data & 0x00FF0000u) >> 0x10;
				if (bytes_per_pixel > 3) {
					target_pixel_bytes[k + 3] = (pixel_data & 0xFF000000u) >> 0x18;
				}
			}
		}
	}
	SDL_UnlockSurface(copy);
}

int G00_VideoGenerateSurfaceSprite(struct G00_Video* video, SDL_Surface* surface, SDL_Color color, unsigned int* out0_index) {
	for (unsigned int i = 0; i <= video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] != NULL) {
			continue;
		}

		video->loaded_surfaces[i] = SDL_DuplicateSurface(surface);
		G00_VideoSurfaceSetNonAlphaPixelColor(video->loaded_surfaces[i], color);

		for (unsigned int j = 0; j < video->config.max_loaded_sprites; j += 1) {
			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
				continue;
			}

			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
			video->loaded_sprites[j].surface_index = i;
			*out0_index = j;
			return 0;
		}
	}

	return 1;
}

int G00_VideoSwapSpriteTextureOrder(struct G00_Video* video, unsigned int a, unsigned int b) {
	if (video->loaded_sprites[a].type != G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE) {
		return 1;
	}

	if (video->loaded_sprites[b].type != G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE) {
		return 2;
	}

	unsigned int temp = video->loaded_sprites[a].surface_index;
	video->loaded_sprites[a].surface_index = video->loaded_sprites[b].surface_index;
	video->loaded_sprites[b].surface_index = temp;

	return 0;
}

void G00_VideoUnloadObject(struct G00_Video* video, unsigned int index) {
	video->loaded_sprites[index].type = G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN;
	video->loaded_sprites[index].surface_index = 0;
	video->loaded_sprites[index].rect = (SDL_FRect) {
		.x = 0.f,
		.y = 0.f,
		.w = 0.f,
		.h = 0.f
	};
}

// int G00_VideoLoadImageFromFile(struct G00_Video* video, const char* path, unsigned int* out0_index) {
// 	for (unsigned int i = 0; i < video->config.max_loaded_textures; i += 1) {
// 		if (video->loaded_textures[i] != NULL) {
// 			continue;
// 		}
//
// 		SDL_Surface* image = IMG_Load(path);
// 		if (image == NULL) {
// 			return -1;
// 		}
//
// 		video->loaded_textures[i] = SDL_CreateTextureFromSurface(video->renderer, image);
// 		if (video->loaded_textures[i] == NULL) {
// 			return -2;
// 		}
// 		SDL_DestroySurface(image);
//
// 		for (unsigned int j = 0; j < video->config.max_loaded_sprites; j += 1) {
// 			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
// 				continue;
// 			}
//
// 			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
// 			*out0_index = video->loaded_sprites[j].index = i;
// 			return 0;
// 		}
// 	}
//
// 	return 1;
// }

int G00_VideoGenerateTextSprite(struct G00_Video* video, unsigned int font_index, const char* text, size_t text_len, SDL_Color color, unsigned int* out0_sprite_index) {
	for (unsigned int i = 1; i < video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] != NULL) {
			continue;
		}

		SDL_Surface* image = TTF_RenderText_Blended(video->loaded_fonts[font_index], text, text_len, color);
		if (image == NULL) {
			return -1;
		}

		video->loaded_surfaces[i] = image;

		for (unsigned int j = 0; j < video->config.max_loaded_sprites; j += 1) {
			if (video->loaded_sprites[j].type != G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN) {
				continue;
			}

			video->loaded_sprites[j].type = G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE;
			video->loaded_sprites[j].surface_index = i;
			*out0_sprite_index = j;
			return 0;
		}
	}

	return 1;
}

int G00_VideoLoadFont(struct G00_Video* video, size_t len, void* mem, float size, unsigned int* out0_index) {
	for (unsigned int i = 0; i < video->config.max_loaded_fonts; i += 1) {
		if (video->loaded_fonts[i] != NULL) {
			continue;
		}

		video->loaded_fonts[i] = TTF_OpenFontIO(SDL_IOFromMem(mem, len), true, size);
		if (video->loaded_fonts[i] == NULL) {
			return -1;
		}

		*out0_index = i;
		return 0;
	}

	return 1;
}

// int G00_VideoLoadFontFromFile(struct G00_Video* video, const char* path, float size, unsigned int* out0_index) {
// 	for (unsigned int i = 0; i < video->config.max_loaded_fonts; i += 1) {
// 		if (video->loaded_fonts[i] != NULL) {
// 			continue;
// 		}
//
// 		video->loaded_fonts[i] = TTF_OpenFont(path, size);
// 		if (video->loaded_fonts[i] == NULL) {
// 			return -1;
// 		}
//
// 		*out0_index = i;
// 		return 0;
// 	}
//
// 	return 1;
// }

void G00_VideoTeardown(struct G00_Video* video) {
	for (unsigned int i = 0; i < video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] == NULL) {
			continue;
		}
		SDL_DestroySurface(video->loaded_surfaces[i]);
		video->loaded_surfaces[i] = NULL;
	}

	for (unsigned int i = 0; i < video->config.max_loaded_fonts; i += 1) {
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
