#ifndef G00_ASSET_H
#define G00_ASSET_H

#include <stdio.h>
#include <stdbool.h>
#include <lzma/7z.h>
#include <lzma/7zFile.h>
#include <lzma/7zAlloc.h>
#include <lzma/7zCrc.h>
#include <lzma/7zBuf.h>

#include "G00_memory.h"

int G00_AssetLoad(const char*, struct G00_MemoryState* out0);

int G00_AssetGenerateLoadOrder();

int G00_AssetLoadFromOrder();

#endif
