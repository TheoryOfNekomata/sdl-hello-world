#include "../G00_command.h"

int G00_ConfigCommandGetHelpArgType(enum G00_CommandArgumentType type, char(* out0_str)[8]) {
	switch (type) {
		case G00_COMMAND_ARGUMENT_TYPE_U8:
			memcpy(out0_str, "u8", 8);
			return 0;
		case G00_COMMAND_ARGUMENT_TYPE_U16:
			memcpy(out0_str, "u16", 8);
			return 0;
		case G00_COMMAND_ARGUMENT_TYPE_U32:
			memcpy(out0_str, "u32", 8);
			return 0;
		case G00_COMMAND_ARGUMENT_TYPE_U64:
			memcpy(out0_str, "u64", 8);
			return 0;
		case G00_COMMAND_ARGUMENT_TYPE_F32:
			memcpy(out0_str, "f32", 8);
			return 0;
		case G00_COMMAND_ARGUMENT_TYPE_STRING:
			memcpy(out0_str, "str", 8);
			return 0;
		default:
			break;
	}

	fprintf(stderr, "Unknown argument type: %d\n", type);
	return -1;
}

int G00_XCommandHelp(char args[255], struct G00_CommandArgumentDefinition arg_defs) {
	char command_name[255];
	char arg_type_str[8];
	unsigned int args_count = 0;
	G00_CommandParseArgs(args, arg_defs, &args_count, &command_name);

	if (args_count == 0) {
		for (unsigned int i = 0; G00_CONFIG_COMMAND_MAPPING[i].name != NULL; i += 1) {
			fprintf(stdout, "%s\n", G00_CONFIG_COMMAND_MAPPING[i].name);
		}
		return 0;
	}

	for (unsigned int i = 0; G00_CONFIG_COMMAND_MAPPING[i].name != NULL; i += 1) {
		if (!strcmpi(G00_CONFIG_COMMAND_MAPPING[i].name, command_name)) {
			fprintf(stdout, "%s", G00_CONFIG_COMMAND_MAPPING[i].name);
			for (unsigned int j = 0; j < G00_CONFIG_COMMAND_MAPPING[i].args.maximum; j += 1) {
				G00_ConfigCommandGetHelpArgType(G00_CONFIG_COMMAND_MAPPING[i].args.defs[j].type, &arg_type_str);
				if (j < G00_CONFIG_COMMAND_MAPPING[i].args.minimum) {
					fprintf(stdout, " <%s:%s>", G00_CONFIG_COMMAND_MAPPING[i].args.defs[j].name, arg_type_str);
				} else {
					fprintf(stdout, " [<%s:%s>]", G00_CONFIG_COMMAND_MAPPING[i].args.defs[j].name, arg_type_str);
				}
			}
			fprintf(stdout, "\n");
			return 0;
		}
	}

	fprintf(stderr, "Unknown command \"%s\"\n", command_name);
	return -1;
}
