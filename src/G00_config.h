#ifndef G00_CONFIG_H
#define G00_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "G00_video.h"
#include "G00_memory.h"

struct G00_Config {
  struct G00_App* app;
  struct G00_MemoryConfig memory;
  struct G00_VideoConfig video;
};

enum G00_ConfigArgumentType {
  G00_CONFIG_ARGUMENT_TYPE_U8,
  G00_CONFIG_ARGUMENT_TYPE_U16,
  G00_CONFIG_ARGUMENT_TYPE_U32,
  G00_CONFIG_ARGUMENT_TYPE_U64,
  G00_CONFIG_ARGUMENT_TYPE_F32,
  G00_CONFIG_ARGUMENT_TYPE_STRING,
};

struct G00_ConfigArgument {
  const char* name;
  enum G00_ConfigArgumentType type;
};

struct G00_ConfigCommandArguments {
  unsigned char minimum;
  unsigned char maximum;
  struct G00_ConfigArgument* defs;
};

typedef int G00_ConfigCommand(char*, struct G00_ConfigCommandArguments, struct G00_Config* out0);

struct G00_ConfigCommandMappingEntry {
  const char* name;
  void* execute_fn;
  struct G00_ConfigCommandArguments args;
};

static struct G00_ConfigCommandMappingEntry G00_CONFIG_COMMAND_MAPPING[];

void G00_ConfigRead(const char*, struct G00_Config* out0);

#endif
