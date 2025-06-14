#include <SDL3/SDL.h>
#include <stdio.h>

const unsigned int SCREEN_WIDTH = 640u;
const unsigned int SCREEN_HEIGHT = 480u;

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}
	window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (window == NULL) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -2;
	}

	screenSurface = SDL_GetWindowSurface(window);
	SDL_FillSurfaceRect(
		screenSurface,
		NULL,
		SDL_MapRGB(SDL_GetPixelFormatDetails(screenSurface->format), NULL, 0xFF, 0xFF, 0xFF)
	);
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
	SDL_Quit();

	return 0;
}
