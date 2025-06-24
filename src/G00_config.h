#ifndef G00_CONFIG_H
#define G00_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "G00_video.h"
#include "G00_memory.h"

struct G00_Config {
  struct G00_MemoryConfig memory;
  struct G00_VideoConfig video;
};

void G00_ConfigRead(const char*, struct G00_Config* out0);

#endif
