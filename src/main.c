#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

const unsigned int SCREEN_WIDTH = 1024u;
const unsigned int SCREEN_HEIGHT = 768u;

void G00_LoadSurface(const char* path, SDL_Surface* dest_surface, SDL_Surface** out0_loaded_surface) {
	SDL_Surface* image = IMG_Load(path);
	if (image == NULL) {
		return;
	}

	*out0_loaded_surface = SDL_ConvertSurface(image, dest_surface->format);
	SDL_DestroySurface(image);
}

void G00_LoadFont(const char* path, float size, TTF_Font** out0_loaded_font) {
	*out0_loaded_font = TTF_OpenFont(path, size);
}

void G00_RenderText(TTF_Font* font, const char* text, size_t text_len, SDL_Color color, SDL_Surface** out0_rendered_text_surface) {
	*out0_rendered_text_surface = TTF_RenderText_Blended(font, text, text_len, color);
}

int G00_InitFramework() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return -1;
	}

	if (!TTF_Init()) {
		return -2;
	}

	return 0;
}

void G00_TeardownFramework() {
	TTF_Quit();
	SDL_Quit();
}

void G00_CreateWindow(SDL_Window** out0_window) {
	*out0_window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
}

int main(int argc, char* argv[]) {
	if (G00_InitFramework()) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window* window;
	G00_CreateWindow(&window);
	if (window == NULL) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -2;
	}

	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

	SDL_Surface* image;
	G00_LoadSurface("assets/images/image.png", screen_surface, &image);
	if (image == NULL) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -3;
	}
	SDL_BlitSurface(
		image, NULL, screen_surface, &(SDL_Rect) {
			.x = (screen_surface->w / 2) - (image->w / 2),
			.y = (screen_surface->h / 2) - (image->h / 2),
			.w = image->w,
			.h = image->h,
		});
	SDL_DestroySurface(image);

	TTF_Font* font;
	G00_LoadFont("assets/fonts/open-sans/OpenSans-Regular.ttf", 48.f, &font);
	if (font == NULL) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return -4;
	}
	SDL_Surface* text;
	G00_RenderText(font, "Hello, world!", 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF }, &text);
	TTF_CloseFont(font);

	SDL_BlitSurface(text, NULL, screen_surface, &(SDL_Rect) {
		.x = (screen_surface->w / 2) - (text->w / 2),
		.y = (screen_surface->h) - (text->h),
		.w = text->w,
		.h = text->h,
	});
	SDL_DestroySurface(text);

	SDL_UpdateWindowSurface(window);
	SDL_Event e;
	bool quit = false;
	while (quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
		}
	}

	SDL_DestroyWindow(window);
	G00_TeardownFramework();

	return 0;
}
