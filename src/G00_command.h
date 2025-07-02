#ifndef G00_COMMAND_H
#define G00_COMMAND_H

#include "G00_config.h"

typedef int G00_Command(char*, struct G00_CommandArgumentDefinition);
typedef int G00_CommandVideo(char*, struct G00_CommandArgumentDefinition, struct G00_Video*);
typedef int G00_CommandMemory(char*, struct G00_CommandArgumentDefinition, struct G00_MemoryState*);

int G00_CommandParseArgs(char[255], struct G00_CommandArgumentDefinition, unsigned int* out0, ...);

#endif
