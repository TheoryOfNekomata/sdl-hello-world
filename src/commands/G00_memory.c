#include "../G00_asset.h"
#include "../G00_memory.h"
#include "../G00_command.h"

int G00_XCommandMemoryAssetLoad(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_MemoryState* memory) {
	char path[255];
	unsigned int args_count = 0;
	if (G00_CommandParseArgs(args, arg_defs, &args_count, path)) {
		return -1;
	}

	char real_path[255];
	sprintf(real_path, "%s/%s", "assets", path);

	fprintf(stdout, "Loading %s...\n", real_path);
	int asset_load_response = G00_AssetLoad(real_path, memory);
	if (asset_load_response < 0) { // TODO fix default asset filenames
		fprintf(stderr, "Unrecoverable error.\n");
		return -2;
	}
	if (asset_load_response > 0) {
		fprintf(stdout, "Loaded with warnings.\n");
		return 1;
	}
	fprintf(stdout, "Loaded successfully.\n");
	return 0;
}

int G00_XCommandMemoryPoolSizeBytes(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_MemoryState* memory) {
	unsigned int args_count = 0;
	if (G00_CommandParseArgs(args, arg_defs, &args_count, &memory->config.pool_size_bytes) < 0) {
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

int G00_XCommandMemoryPoolMaxEntries(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_MemoryState* memory) {
	unsigned int args_count = 0;
	if (G00_CommandParseArgs(args, arg_defs, &args_count, &memory->config.pool_max_entries) < 0) {
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
