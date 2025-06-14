#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

const unsigned int SCREEN_WIDTH = 1024u;
const unsigned int SCREEN_HEIGHT = 768u;

void G00_LoadSurface(SDL_Renderer* renderer, const char* path, SDL_Texture** out0_loaded_surface) {
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

enum G00_ProcedureResult_Main {
	G00_PROCEDURE_RESULT_MAIN_OK = 0,
	G00_PROCEDURE_RESULT_MAIN_FRAMEWORK_ERROR = -1,
	G00_PROCEDURE_RESULT_MAIN_VIDEO_ERROR = -2,
	G00_PROCEDURE_RESULT_MAIN_ASSET_ERROR = -3,
};

int main(int argc, char* argv[]) {
	if (G00_InitFramework()) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return G00_PROCEDURE_RESULT_MAIN_FRAMEWORK_ERROR;
	}

	SDL_Window* window;
	G00_CreateWindow(&window);
	if (window == NULL) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return G00_PROCEDURE_RESULT_MAIN_VIDEO_ERROR;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL) {
		fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return G00_PROCEDURE_RESULT_MAIN_VIDEO_ERROR;
	}

	SDL_Texture* image;
	G00_LoadSurface(renderer, "assets/images/image.png", &image);
	if (image == NULL) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return G00_PROCEDURE_RESULT_MAIN_ASSET_ERROR;
	}

	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, image, NULL, &(SDL_FRect) {
			.x = (SCREEN_WIDTH / 2.f) - (image->w / 2.f),
			.y = (SCREEN_HEIGHT / 2.f) - (image->h / 2.f),
			.w = image->w,
			.h = image->h,
		});
	SDL_DestroyTexture(image);

	TTF_Font* font;
	G00_LoadFont("assets/fonts/open-sans/OpenSans-Regular.ttf", 48.f, &font);
	if (font == NULL) {
		fprintf(stderr, "Unable to load font! SDL_Error: %s\n", SDL_GetError());
		return G00_PROCEDURE_RESULT_MAIN_ASSET_ERROR;
	}
	SDL_Texture* text;
	G00_RenderText(renderer, font, "Hello, world!", 13, (SDL_Color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF }, &text);
	TTF_CloseFont(font);
	SDL_RenderTexture(renderer, text, NULL, &(SDL_FRect) {
		.x = (SCREEN_WIDTH / 2.f) - (text->w / 2.f),
		.y = (SCREEN_HEIGHT) - (text->h),
		.w = text->w,
		.h = text->h,
	});
	SDL_DestroyTexture(text);

	SDL_RenderPresent(renderer);
	SDL_Event e;
	bool quit = false;
	while (quit == false) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}


		}
	}

	SDL_DestroyRenderer(renderer);
	renderer = NULL;

	SDL_DestroyWindow(window);
	window = NULL;
	G00_TeardownFramework();

	return G00_PROCEDURE_RESULT_MAIN_OK;
}
