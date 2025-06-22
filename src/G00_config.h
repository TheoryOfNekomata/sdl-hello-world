#ifndef G00_CONFIG_H
#define G00_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "G00_video.h"

struct G00_Config {
  struct G00_VideoConfig video;
};

enum G00_ConfigParserMode : unsigned char {
  G00_CONFIG_PARSER_MODE_COMMAND,
  G00_CONFIG_PARSER_MODE_ARGS,
  G00_CONFIG_PARSER_MODE_COMMENT,
  G00_CONFIG_PARSER_MODE_ESCAPE,
};

enum G00_ConfigCommandType : unsigned char {
  G00_CONFIG_COMMAND_TYPE_UNKNOWN,
  G00_CONFIG_COMMAND_TYPE_SCREEN_SIZE,
  G00_CONFIG_COMMAND_TYPE_IDEAL_FPS,
};

void G00_ConfigRead(const char*, struct G00_Config* out0);

#endif
