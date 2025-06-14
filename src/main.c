#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

const unsigned int SCREEN_WIDTH = 1024u;
const unsigned int SCREEN_HEIGHT = 768u;

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}
	SDL_Window* window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (window == NULL) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -2;
	}

	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

	SDL_Surface* image = IMG_Load("assets/image.png");
	if (image == NULL) {
		fprintf(stderr, "Unable to load image! SDL_Error: %s\n", SDL_GetError());
		return -2;
	}

	SDL_Surface* optimized_surface = SDL_ConvertSurface(image, screen_surface->format);
	SDL_BlitSurface(optimized_surface, NULL, screen_surface, NULL);
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

	SDL_DestroySurface(optimized_surface);
	SDL_DestroySurface(image);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
