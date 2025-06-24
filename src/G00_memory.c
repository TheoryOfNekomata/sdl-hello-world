#include "G00_memory.h"

int G00_MemoryRetrieveIndex(struct G00_MemoryState* memory, unsigned char* name, unsigned int* out0_index) {
	if (memory == NULL) {
		return -1;
	}

	if (name == NULL) {
		return -2;
	}

	for (unsigned int i = 0; i < G00_MEMORY_MAX_ENTRIES; i += 1) {
		if (!strcmp(memory->entries[i].name, name)) {
			*out0_index = i;
			return 0;
		}
	}

	return 1;
}
