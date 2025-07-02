#include "G00_memory.h"

#include <stdio.h>
#include <string.h>

int G00_MemoryInit(struct G00_MemoryState* memory) {
	memory->data = malloc(memory->config.pool_size_bytes);
	if (memory->data == NULL) {
		fprintf(stderr, "Could not allocate pool.\n");
		return -1;
	}
	memset(memory->data, 0, memory->config.pool_size_bytes);

	size_t entries_len = sizeof(struct G00_MemoryOffsetEntry) * memory->config.pool_max_entries;
	memory->entries = malloc(entries_len);
	if (memory->entries == NULL) {
		fprintf(stderr, "Could not allocate pool entries.\n");
		return -2;
	}
	memset(memory->entries, 0, entries_len);
	memory->asset_data_processed_bytes = 0;
	return 0;
}

int G00_MemoryRetrieveIndex(struct G00_MemoryState* memory, char* name, unsigned int* out0_index) {
	if (memory == NULL) {
		return -1;
	}

	if (name == NULL) {
		return -2;
	}

	for (unsigned int i = 0; i < memory->config.pool_max_entries; i += 1) {
		if (!strcmp(memory->entries[i].name, name)) {
			*out0_index = i;
			return 0;
		}
	}

	return 1;
}

void G00_MemoryTeardown(struct G00_MemoryState* memory) {
	free(memory->data);
	memory->data = NULL;

	free(memory->entries);
	memory->entries = NULL;
}
