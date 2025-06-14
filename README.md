# sdl-hello-world

A minimal starting project for SDL in C.

## Instructions

### Windows

Requirements:

- Visual Studio 2022 or newer
- CMake 3.31 or newer

1. Create a [`thirdparty`](./thirdparty) folder in the root of the project.
2. Download the [latest SDL development build for Visual Studio](https://github.com/libsdl-org/SDL/releases/latest).
3. Extract the SDL build to the top level of [`thirdparty`](./thirdparty) while preserving the internal directory structure inside the archive (do not create an enclosing folder upon extracting).
4. Compile the project using CMake.

You should have the following directory structure:

```
sdl-hello-world/
  src/
  thirdparty/
    SDL3-3.X.X/
      cmake/
      include/
      lib/
      ...other SDL files here
  .gitignore
  CMakeLists.txt
  README.md
```
