#ifndef G00_ASSET_H
#define G00_ASSET_H

#if _WINDOWS
#include <dirent/dirent.h>
#else
#include <dirent.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <lzma/7z.h>
#include <lzma/7zFile.h>
#include <lzma/7zAlloc.h>
#include <lzma/7zCrc.h>
#include <lzma/7zBuf.h>

#include "G00_memory.h"

struct G00_AssetConfig {
	char assets_dir_relative_path[255];
};

int G00_AssetGenerateLoadOrder(const char*);

int G00_AssetLoadFromOrder(const char*, struct G00_MemoryState* out0);

#endif
