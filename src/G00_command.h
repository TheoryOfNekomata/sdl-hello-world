#ifndef G00_COMMAND_H
#define G00_COMMAND_H

#include "G00_config.h"

enum G00_CommandArgumentType : unsigned int {
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

struct G00_CommandEntry {
	const char* name;
	void* execute_fn;
	struct G00_CommandArgumentDefinition args;
};

typedef int G00_Command(char*, struct G00_CommandArgumentDefinition, struct G00_App*);
typedef int G00_CommandVideo(char*, struct G00_CommandArgumentDefinition, struct G00_Video*);
typedef int G00_CommandMemory(char*, struct G00_CommandArgumentDefinition, struct G00_MemoryState*);
typedef int G00_CommandUI(char*, struct G00_CommandArgumentDefinition, struct G00_UIState*);

int G00_CommandParseArgs(char[255], struct G00_CommandArgumentDefinition, unsigned int* out0, ...);

#endif
