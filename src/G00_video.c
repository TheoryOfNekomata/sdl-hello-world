#include <SDL3_image/SDL_image.h>

#include "G00_video.h"

#include "G00_memory.h"

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

	size_t loaded_fonts_len = sizeof(TTF_Font*) * (video->config.max_loaded_fonts + 1);
	video->loaded_fonts = malloc(loaded_fonts_len);
	memset(video->loaded_fonts, 0, loaded_fonts_len);
	size_t loaded_surfaces_len = sizeof(SDL_Surface*) * (video->config.max_loaded_surfaces + 1);
	video->loaded_surfaces = malloc(loaded_surfaces_len);
	memset(video->loaded_surfaces, 0, loaded_surfaces_len);
	size_t loaded_sprites_len = sizeof(struct G00_VideoImageSprite) * (video->config.max_loaded_sprites + 1);
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

void G00_VideoUpdate(struct G00_Video* video, unsigned long app_ticks) {
	float ticks_elapsed = (float) (app_ticks - video->ticks);
	if (ticks_elapsed <= video->config.millis_per_tick) {
		return;
	}

	SDL_SetRenderDrawColor(video->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(video->renderer);
	for (unsigned int i = video->config.max_loaded_sprites; i > 0; i -= 1) {
	// for (unsigned int i = 1; i <= video->config.max_loaded_sprites - 1; i += 1) {
		switch (video->loaded_sprites[i].base.type) {
			case G00_VIDEO_SPRITE_TYPE_IMAGE: {
				SDL_Surface* orig_surface = video->loaded_surfaces[video->loaded_sprites[i].image.orig_surface_index];
				SDL_Surface* final_surface = orig_surface;

				bool has_processing = video->loaded_sprites[i].image.processing_nodes != NULL;
				if (has_processing) {
					bool is_processed_surface_cached = video->loaded_sprites[i].image.processed_surface_index != video->loaded_sprites[i].image.orig_surface_index;

					if (!is_processed_surface_cached) {
						do {
							video->loaded_sprites[i].image.processed_surface_index += 1;
						} while (
							video->loaded_surfaces[video->loaded_sprites[i].image.processed_surface_index] != NULL
							&& video->loaded_sprites[i].image.processed_surface_index <= video->config.max_loaded_surfaces
						);

						if (video->loaded_sprites[i].image.processed_surface_index > video->config.max_loaded_surfaces) {
							video->loaded_sprites[i].image.processed_surface_index = video->loaded_sprites[i].image.orig_surface_index;
						}

						video->loaded_surfaces[video->loaded_sprites[i].image.processed_surface_index] = SDL_DuplicateSurface(orig_surface);
						struct G00_ListNode* processing_node = video->loaded_sprites[i].image.processing_nodes;
						if (processing_node != NULL) {
							do {
								switch (((union G00_VideoProcessingNode*) processing_node->data)->base.type) {
									case G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR:
										G00_VideoSurfaceSetNonAlphaPixelColor(
											video->loaded_surfaces[video->loaded_sprites[i].image.processed_surface_index],
											((union G00_VideoProcessingNode*) processing_node->data)->fill_color.color
										);
										break;
									default:
										break;
								}
								processing_node = processing_node->next;
							} while (processing_node != NULL);
						}
					}
					final_surface = video->loaded_surfaces[video->loaded_sprites[i].image.processed_surface_index];
				}

				SDL_Texture* texture = SDL_CreateTextureFromSurface(video->renderer, final_surface);
				if (texture != NULL) {
					SDL_RenderTexture(
						video->renderer,
						texture,
						NULL,
						&video->loaded_sprites[i].image.rect
					);
					SDL_DestroyTexture(texture);
				}

				break;
			}

			case G00_VIDEO_SPRITE_TYPE_TEXT: {
					SDL_Surface* orig_surface = video->loaded_surfaces[video->loaded_sprites[i].text.orig_surface_index];
				SDL_Surface* final_surface = orig_surface;

				bool has_processing = video->loaded_sprites[i].text.processing_nodes != NULL;
				if (has_processing) {
					bool is_processed_surface_cached = video->loaded_sprites[i].text.processed_surface_index != video->loaded_sprites[i].text.orig_surface_index;

					if (!is_processed_surface_cached) {
						do {
							video->loaded_sprites[i].text.processed_surface_index += 1;
						} while (
							video->loaded_surfaces[video->loaded_sprites[i].text.processed_surface_index] != NULL
							&& video->loaded_sprites[i].text.processed_surface_index <= video->config.max_loaded_surfaces
						);

						if (video->loaded_sprites[i].text.processed_surface_index > video->config.max_loaded_surfaces) {
							video->loaded_sprites[i].text.processed_surface_index = video->loaded_sprites[i].text.orig_surface_index;
						}

						video->loaded_surfaces[video->loaded_sprites[i].text.processed_surface_index] = SDL_DuplicateSurface(orig_surface);
						struct G00_ListNode* processing_node = video->loaded_sprites[i].text.processing_nodes;
						if (processing_node != NULL) {
							do {
								switch (((union G00_VideoProcessingNode*) processing_node->data)->base.type) {
									case G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR:
										G00_VideoSurfaceSetNonAlphaPixelColor(
											video->loaded_surfaces[video->loaded_sprites[i].text.processed_surface_index],
											((union G00_VideoProcessingNode*) processing_node->data)->fill_color.color
										);
										break;
									default:
										break;
								}
								processing_node = processing_node->next;
							} while (processing_node != NULL);
						}
					}
					final_surface = video->loaded_surfaces[video->loaded_sprites[i].text.processed_surface_index];
				}

				SDL_Texture* texture = SDL_CreateTextureFromSurface(video->renderer, final_surface);
				if (texture != NULL) {
					SDL_RenderTexture(
						video->renderer,
						texture,
						NULL,
						&video->loaded_sprites[i].text.rect
					);
					SDL_DestroyTexture(texture);
				}

				break;
			}

			default:
				break;
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
	for (unsigned int i = 1; i <= video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] != NULL) {
			continue;
		}

		SDL_Surface* image = IMG_Load_IO(SDL_IOFromMem(mem, len), true);
		if (image == NULL) {
			return -1;
		}

		video->loaded_surfaces[i] = image;
		for (unsigned int j = 1; j <= video->config.max_loaded_sprites; j += 1) {
			if (video->loaded_sprites[j].base.type != G00_VIDEO_SPRITE_TYPE_BASE) {
				continue;
			}

			video->loaded_sprites[j].image.type = G00_VIDEO_SPRITE_TYPE_IMAGE;
			video->loaded_sprites[j].image.orig_surface_index = i;
			video->loaded_sprites[j].image.processed_surface_index = i;
			video->loaded_sprites[j].image.processing_nodes = NULL;
			*out0_index = j;
			return 0;
		}
	}

	return 1;
}

int G00_VideoDuplicateSprite(struct G00_Video* video, unsigned int src_index, unsigned int* out0_dest_index) {
	unsigned int new_sprite_index = 0;
	do {
		new_sprite_index += 1;
	} while (video->loaded_sprites[new_sprite_index].base.type != G00_VIDEO_SPRITE_TYPE_BASE && new_sprite_index <= video->config.max_loaded_sprites);
	if (new_sprite_index > video->config.max_loaded_sprites) {
		return 1;
	}

	video->loaded_sprites[new_sprite_index].image.orig_surface_index = video->loaded_sprites[src_index].image.orig_surface_index;
	video->loaded_sprites[new_sprite_index].image.processed_surface_index = video->loaded_sprites[src_index].image.processed_surface_index;
	video->loaded_sprites[new_sprite_index].image.type = video->loaded_sprites[src_index].image.type;
	video->loaded_sprites[new_sprite_index].image.rect = video->loaded_sprites[src_index].image.rect;
	video->loaded_sprites[new_sprite_index].image.processing_nodes = NULL;
	*out0_dest_index = new_sprite_index;

	return 0;
}

int G00_VideoSwapSpriteTextureOrder(struct G00_Video* video, unsigned int a, unsigned int b) {
	if (video->loaded_sprites[a].base.type != G00_VIDEO_SPRITE_TYPE_IMAGE) {
		return 1;
	}

	if (video->loaded_sprites[b].base.type != G00_VIDEO_SPRITE_TYPE_IMAGE) {
		return 2;
	}

	unsigned int temp = video->loaded_sprites[a].image.orig_surface_index;
	video->loaded_sprites[a].image.orig_surface_index = video->loaded_sprites[b].image.orig_surface_index;
	video->loaded_sprites[b].image.orig_surface_index = temp;

	temp = video->loaded_sprites[a].image.processed_surface_index;
	video->loaded_sprites[a].image.processed_surface_index = video->loaded_sprites[b].image.processed_surface_index;
	video->loaded_sprites[b].image.processed_surface_index = temp;

	return 0;
}

void G00_VideoUnloadObject(struct G00_Video* video, unsigned int index) {
	if (video->loaded_sprites[index].base.type == G00_VIDEO_SPRITE_TYPE_IMAGE) {
		if (video->loaded_surfaces[video->loaded_sprites[index].image.orig_surface_index] != NULL) {
			SDL_DestroySurface(video->loaded_surfaces[video->loaded_sprites[index].image.orig_surface_index]);
			video->loaded_surfaces[video->loaded_sprites[index].image.orig_surface_index] = NULL;
		}
		if (video->loaded_surfaces[video->loaded_sprites[index].image.processed_surface_index] != NULL) {
			SDL_DestroySurface(video->loaded_surfaces[video->loaded_sprites[index].image.processed_surface_index]);
			video->loaded_surfaces[video->loaded_sprites[index].image.processed_surface_index] = NULL;
		}
		video->loaded_sprites[index].image.orig_surface_index = 0;
		video->loaded_sprites[index].image.processed_surface_index = 0;
		video->loaded_sprites[index].image.rect = (SDL_FRect) {
			.x = 0.f,
			.y = 0.f,
			.w = 0.f,
			.h = 0.f
		};
	}
	video->loaded_sprites[index].base.type = G00_VIDEO_SPRITE_TYPE_BASE;
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

int G00_VideoGenerateTextSprite(struct G00_Video* video, unsigned int font_index, const char* text, size_t text_len, unsigned int* out0_sprite_index) {
	unsigned int joaat_hash = 0;
	G00_MemoryJoaat(text, &joaat_hash);

	for (unsigned int k = 1; k <= video->config.max_loaded_surfaces; k += 1) {
		if (video->loaded_surfaces[k] == NULL) {
			continue;
		}
		if (video->loaded_sprites[k].text.joaat_hash == joaat_hash) {
			*out0_sprite_index = k;
			return 0;
		}
	}

	for (unsigned int i = 1; i <= video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] != NULL) {
			continue;
		}

		SDL_Surface* image = TTF_RenderText_Blended(video->loaded_fonts[font_index], text, text_len, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF });
		if (image == NULL) {
			return -1;
		}

		video->loaded_surfaces[i] = image;

		for (unsigned int j = 1; j <= video->config.max_loaded_sprites; j += 1) {
			if (video->loaded_sprites[j].base.type != G00_VIDEO_SPRITE_TYPE_BASE) {
				continue;
			}

			video->loaded_sprites[j].text.type = G00_VIDEO_SPRITE_TYPE_TEXT;
			video->loaded_sprites[j].text.orig_surface_index = i;
			video->loaded_sprites[j].text.processed_surface_index = i;
			video->loaded_sprites[j].text.processing_nodes = NULL;
			video->loaded_sprites[j].text.joaat_hash = joaat_hash;
			*out0_sprite_index = j;
			return 0;
		}
	}

	return 1;
}

int G00_VideoLoadFont(struct G00_Video* video, size_t len, void* mem, float size, unsigned int* out0_index) {
	for (unsigned int i = 1; i <= video->config.max_loaded_fonts; i += 1) {
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
	for (unsigned int i = 1; i <= video->config.max_loaded_surfaces; i += 1) {
		if (video->loaded_surfaces[i] == NULL) {
			continue;
		}
		SDL_DestroySurface(video->loaded_surfaces[i]);
		video->loaded_surfaces[i] = NULL;
	}

	for (unsigned int i = 1; i <= video->config.max_loaded_fonts; i += 1) {
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
