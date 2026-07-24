#ifndef G00_VIDEO_H
#define G00_VIDEO_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "G00_list.h"

struct G00_App;

struct G00_VideoConfig {
	unsigned int screen_width;
	unsigned int screen_height;
	float aspect_ratio;
	float frames_per_second;
	float millis_per_tick;
	unsigned char max_loaded_surfaces;
	unsigned char max_loaded_fonts;
	unsigned char max_loaded_sprites;
};

enum G00_VideoSpriteType : unsigned char {
	G00_VIDEO_SPRITE_TYPE_BASE,
	G00_VIDEO_SPRITE_TYPE_IMAGE,
	G00_VIDEO_SPRITE_TYPE_TEXT,
};

enum G00_VideoProcessingNodeType : unsigned char {
	G00_VIDEO_PROCESSING_NODE_TYPE_NOOP,
	G00_VIDEO_PROCESSING_NODE_TYPE_FILL_COLOR,
};

struct G00_VideoNoopProcessingNode {
	enum G00_VideoProcessingNodeType type;
};

struct G00_VideoFillColorProcessingNode {
	enum G00_VideoProcessingNodeType type;
	SDL_Color color;
};

union G00_VideoProcessingNode {
	struct G00_VideoNoopProcessingNode noop;
	struct G00_VideoFillColorProcessingNode fill_color;
};

struct G00_VideoBaseSprite {
	enum G00_VideoSpriteType type;
};

struct G00_VideoImageSprite {
	enum G00_VideoSpriteType type;
	unsigned int orig_surface_index;
	unsigned int processed_surface_index;
	SDL_FRect rect;
	struct G00_ListNode* processing_nodes;
};

struct G00_VideoTextSprite {
	enum G00_VideoSpriteType type;
	unsigned int orig_surface_index;
	unsigned int processed_surface_index;
	SDL_FRect rect;
	struct G00_ListNode* processing_nodes;
	void* text;
};

union G00_VideoSprite {
	struct G00_VideoBaseSprite base;
	struct G00_VideoImageSprite image;
	struct G00_VideoTextSprite text;
};

struct G00_Video {
	struct G00_VideoConfig config;
	unsigned long long ticks;
	SDL_Window* window;
	SDL_Renderer* renderer;
	struct G00_App* app;
	SDL_Surface** loaded_surfaces;
	TTF_Font** loaded_fonts;
	union G00_VideoSprite* loaded_sprites;
};

void G00_VideoTeardown(struct G00_Video*);

enum G00_VideoInitResult : int {
	G00_VIDEO_INIT_RESULT_OK = 0,
	G00_VIDEO_INIT_RESULT_UNKNOWN_VIDEO_STATE = -1,
	G00_VIDEO_INIT_RESULT_UNBOUND_VIDEO_STATE = -2,
	G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_WINDOW = -3,
	G00_VIDEO_INIT_RESULT_COULD_NOT_CREATE_RENDERER = -4,
};

enum G00_VideoInitResult G00_VideoInit(struct G00_Video*);

void G00_VideoUpdate(struct G00_Video*, unsigned long);

int G00_VideoUpdateWindow(struct G00_Video*);

// int G00_VideoLoadImageFromFile(struct G00_Video*, const char*, unsigned int* out0);

int G00_VideoLoadImageSprite(struct G00_Video*, size_t, void*, unsigned int* out0);

int G00_VideoDuplicateSprite(struct G00_Video*, unsigned int, unsigned int* out0);

int G00_VideoGenerateTextSprite(struct G00_Video*, unsigned int, const char*, size_t, SDL_Color, void*, unsigned int* out0);

void G00_VideoUnloadObject(struct G00_Video*, unsigned int);

int G00_VideoSwapSpriteTextureOrder(struct G00_Video*, unsigned int, unsigned int);

// int G00_VideoLoadFontFromFile(struct G00_Video*, const char*, float, unsigned int* out0);

int G00_VideoLoadFont(struct G00_Video*, size_t, void*, float, unsigned int* out0);

#endif
