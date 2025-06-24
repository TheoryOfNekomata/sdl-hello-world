#ifndef G00_VIDEO_H
#define G00_VIDEO_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

struct G00_App;

struct G00_VideoConfig {
	unsigned int screen_width;
	unsigned int screen_height;
	float aspect_ratio;
	float frames_per_second;
	float millis_per_tick;
	unsigned char max_loaded_textures;
	unsigned char max_loaded_fonts;
	unsigned char max_loaded_sprites;
};

enum G00_VideoLoadedObjectType : unsigned char {
	G00_VIDEO_LOADED_OBJECT_TYPE_UNKNOWN,
	G00_VIDEO_LOADED_OBJECT_TYPE_TEXTURE,
};

struct G00_VideoSprite {
	enum G00_VideoLoadedObjectType type;
	unsigned int index;
	SDL_FRect rect;
};

struct G00_Video {
	struct G00_VideoConfig config;
	unsigned long long ticks;
	SDL_Window* window;
	SDL_Renderer* renderer;
	struct G00_App* app;
	SDL_Texture** loaded_textures;
	TTF_Font** loaded_fonts;
	struct G00_VideoSprite* loaded_sprites;
};

void G00_VideoTeardown(struct G00_Video*);

enum G00_VideoInitResult {
	G00_VIDEO_INIT_RESULT_OK = 0,
	G00_VIDEO_INIT_RESULT_UNKNOWN_VIDEO_STATE = -1,
	G00_VIDEO_INIT_RESULT_UNBOUND_VIDEO_STATE = -2,
	G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_WINDOW = -3,
	G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_RENDERER = -3,
};

enum G00_VideoInitResult G00_VideoInit(struct G00_Video*, struct G00_VideoConfig);

void G00_VideoUpdate(struct G00_Video*, unsigned long);

int G00_VideoLoadImageFromFile(struct G00_Video*, const char*, unsigned int* out0);

int G00_VideoLoadImageFromMemory(struct G00_Video*, size_t, void*, unsigned int* out0);

int G00_VideoLoadText(struct G00_Video*, unsigned int, const char*, size_t, SDL_Color, unsigned int* out0);

void G00_VideoUnloadObject(struct G00_Video*, unsigned int);

int G00_VideoLoadFontFromFile(struct G00_Video*, const char*, float, unsigned int* out0);

int G00_VideoLoadFontFromMemory(struct G00_Video* video, size_t len, void* mem, float size, unsigned int* out0_index);

#endif
