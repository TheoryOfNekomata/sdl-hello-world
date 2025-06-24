#ifndef G00_MEMORY_H
#define G00_MEMORY_H

#include <string.h>

static const unsigned int G00_MEMORY_MAX_ENTRIES = 256u;

struct G00_MemoryOffsetEntry {
	unsigned char* name;
	unsigned long long offset;
	unsigned long long len;
};

struct G00_MemoryState {
	struct G00_MemoryOffsetEntry entries[G00_MEMORY_MAX_ENTRIES];
	char* data;
};

int G00_MemoryRetrieveIndex(struct G00_MemoryState*, unsigned char*, unsigned int* out0);

#endif
