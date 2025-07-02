#include "../G00_command.h"

int G00_CommandMemoryPoolSizeBytes(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->memory.pool_size_bytes) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Memory pool size is %llu bytes\n", out0_config->memory.pool_size_bytes);
		return 0;
	}


	if (!(10485760u <= out0_config->memory.pool_size_bytes && out0_config->memory.pool_size_bytes <= 2147483648u)) {
		out0_config->memory.pool_size_bytes = 10485760u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_CommandMemoryPoolMaxEntries(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->memory.pool_max_entries) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Memory pool maximum entries count is %u\n", out0_config->memory.pool_max_entries);
		return 0;
	}

	if (!(256u <= out0_config->memory.pool_max_entries && out0_config->memory.pool_max_entries <= 131072u)) {
		out0_config->memory.pool_max_entries = 256u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}
