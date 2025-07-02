#include "../G00_command.h"

int G00_CommandMemoryPoolSizeBytes(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_MemoryState* memory) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &memory->config.pool_size_bytes) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Memory pool size is %llu bytes\n", memory->config.pool_size_bytes);
		return 0;
	}


	if (!(10485760u <= memory->config.pool_size_bytes && memory->config.pool_size_bytes <= 2147483648u)) {
		memory->config.pool_size_bytes = 10485760u;
		// TODO resize memory
		return 1;
	}

	// TODO resize memory
	return 0;
}

int G00_CommandMemoryPoolMaxEntries(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_MemoryState* memory) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &memory->config.pool_max_entries) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Memory pool maximum entries count is %u\n", memory->config.pool_max_entries);
		return 0;
	}

	if (!(256u <= memory->config.pool_max_entries && memory->config.pool_max_entries <= 131072u)) {
		memory->config.pool_max_entries = 256u;

		// TODO resize memory
		return 1;
	}

	// TODO resize memory
	return 0;
}
