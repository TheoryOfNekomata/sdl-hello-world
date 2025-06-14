# sdl-hello-world

A minimal starting project for SDL in C.

## Instructions

### Windows

Requirements:

- Visual Studio 2022 or newer
- CMake 3.31 or newer

1. Create a [`thirdparty`](./thirdparty) folder in the root of the project.
2. Download the latest SDL development builds for Visual Studio:
   - [SDL](https://github.com/libsdl-org/SDL/releases/latest)
   - [SDL_image](https://github.com/libsdl-org/SDL_image/releases/latest)
   - [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/latest)
3. Extract the SDL builds to the top level of [`thirdparty`](./thirdparty). Observe the expected directory structure below.
4. Compile the project using CMake.

You should have the following directory structure:

```
sdl-hello-world/
  src/
  thirdparty/
    SDL3/
      cmake/
      include/
      lib/
      ...other SDL files here
    SDL3_image/
      cmake/
      include/
      lib/
      ...other SDL_image files here
    SDL3_ttf/
      cmake/
      include/
      lib/
      ...other SDL_ttf files here
  .gitignore
  CMakeLists.txt
  README.md
```
