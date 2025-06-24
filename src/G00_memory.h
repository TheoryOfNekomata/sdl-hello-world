#ifndef G00_MEMORY_H
#define G00_MEMORY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct G00_MemoryConfig {
	unsigned long long pool_size_bytes;
	unsigned int pool_max_entries;
};

struct G00_MemoryOffsetEntry {
	unsigned char* name;
	unsigned long long offset;
	unsigned long long len;
	unsigned char* reference_asset_path;
};

struct G00_MemoryState {
	struct G00_MemoryConfig config;
	struct G00_MemoryOffsetEntry* entries;
	char* data;
	size_t asset_data_processed_bytes;
};

int G00_MemoryInit(struct G00_MemoryState*, struct G00_MemoryConfig);

void G00_MemoryTeardown(struct G00_MemoryState*);

int G00_MemoryRetrieveIndex(struct G00_MemoryState*, unsigned char*, unsigned int* out0);

#endif
