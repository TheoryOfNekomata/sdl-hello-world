#ifndef G00_CONFIG_H
#define G00_CONFIG_H

#include <stdio.h>
#include <string.h>

#include "G00_database.h"
#include "G00_video.h"
#include "G00_memory.h"

struct G00_App {
  sqlite3* db;
  unsigned long long ticks;
  struct G00_Video video;
  struct G00_MemoryState memory;
};

enum G00_CommandArgumentType {
  G00_COMMAND_ARGUMENT_TYPE_U8 = 0x1,
  G00_COMMAND_ARGUMENT_TYPE_U16 = 0x2,
  G00_COMMAND_ARGUMENT_TYPE_U32 = 0x4,
  G00_COMMAND_ARGUMENT_TYPE_U64 = 0x8,
  G00_COMMAND_ARGUMENT_TYPE_F32 = 0x10,
  G00_COMMAND_ARGUMENT_TYPE_STRING = 0x20,
};

struct G00_CommandArgument {
  const char* name;
  enum G00_CommandArgumentType type;
};

struct G00_CommandArgumentDefinition {
  unsigned char minimum;
  unsigned char maximum;
  struct G00_CommandArgument* defs;
};

typedef int G00_Command(char*, struct G00_CommandArgumentDefinition);
typedef int G00_CommandVideo(char*, struct G00_CommandArgumentDefinition, struct G00_Video*);
typedef int G00_CommandMemory(char*, struct G00_CommandArgumentDefinition, struct G00_MemoryState*);

struct G00_ConfigCommandMappingEntry {
  const char* name;
  void* execute_fn;
  struct G00_CommandArgumentDefinition args;
};

static struct G00_ConfigCommandMappingEntry G00_CONFIG_COMMAND_MAPPING[];

void G00_ConfigExecuteScript(const char*, struct G00_App*);

#endif
