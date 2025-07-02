#include "../G00_command.h"

int G00_ConfigParseArgs(char input[255], struct G00_ConfigCommandArguments arg_defs, unsigned int* out0_parsed_args, ...) {
	char buffer[255];
	strncpy(buffer, input, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	char *p = buffer;
	va_list args;
	va_start(args, arg_defs);

	unsigned char parsed_args = 0;
	for (size_t i = 0; ; i += 1) {
		while (isspace((unsigned char)*p)) {
			p += 1;
		}

		if (*p == '\0') {
			if (parsed_args == 0) {
				*out0_parsed_args = parsed_args;
				va_end(args);
				return 1;
			}

			if (parsed_args < arg_defs.minimum) {
				fprintf(stderr, "Not enough args: expected %u, received %u.\n", arg_defs.minimum, parsed_args);
				va_end(args);
				return -1;
			}

			if (parsed_args > arg_defs.maximum) {
				fprintf(stderr, "Too many args: expected %u, received %u.\n", arg_defs.maximum, parsed_args);
				va_end(args);
				return -1;
			}

			break;
		}

		char* endptr = NULL;
		errno = 0;

		if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_U32) {
			unsigned int* out = va_arg(args, unsigned int*);
			unsigned int val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u32 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = val;
		} else if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_U8) {
			unsigned char* out = va_arg(args, unsigned char*);
			unsigned long val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u8 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = (unsigned char) val;
		} else if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_U16) {
			unsigned short* out = va_arg(args, unsigned short*);
			unsigned long val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u16 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = (unsigned short) val;
		} else if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_U64) {
			unsigned long long* out = va_arg(args, unsigned long long*);
			unsigned long long val = strtoull(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u64 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = val;
		} else if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_STRING) {
			char *dest = va_arg(args, char*);
			unsigned int maxlen = 255;

			// Find end of token
			endptr = p;
			while (*endptr && !isspace((unsigned char)*endptr)) {
				endptr += 1;
			}

			size_t len = endptr - p;
			if ((int)len >= maxlen) {
				fprintf(stderr, "String too long at position %zu (max %d)\n", i, maxlen);
				va_end(args);
				return -2;
			}

			strncpy(dest, p, len);
			dest[len] = '\0'; // null-terminate
		} else if (arg_defs.defs[i].type == G00_CONFIG_ARGUMENT_TYPE_F32) {
			float* out = va_arg(args, float*);
			float val = strtof(p, &endptr);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse f32 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = val;
		} else {
			fprintf(stderr, "Unknown format specifier: %u\n", arg_defs.defs[i].type);
			va_end(args);
			return -3;
		}

		p = endptr;
		parsed_args += 1;
	}

	// Check for trailing non-whitespace (optional strict mode)
	while (isspace((unsigned char)*p)) {
		p += 1;
	}

	if (*p != '\0') {
		fprintf(stderr, "Extra unparsed data found: \"%s\"\n", p);
		va_end(args);
		return -4;
	}

	*out0_parsed_args = parsed_args;
	va_end(args);
	return 0;
}

int G00_ConfigCommandGetHelpArgType(enum G00_ConfigArgumentType type, char(* out0_str)[8]) {
	switch (type) {
		case G00_CONFIG_ARGUMENT_TYPE_U8:
			memcpy(out0_str, "u8", 8);
			return 0;
		case G00_CONFIG_ARGUMENT_TYPE_U16:
			memcpy(out0_str, "u16", 8);
			return 0;
		case G00_CONFIG_ARGUMENT_TYPE_U32:
			memcpy(out0_str, "u32", 8);
			return 0;
		case G00_CONFIG_ARGUMENT_TYPE_U64:
			memcpy(out0_str, "u64", 8);
			return 0;
		case G00_CONFIG_ARGUMENT_TYPE_F32:
			memcpy(out0_str, "f32", 8);
			return 0;
		case G00_CONFIG_ARGUMENT_TYPE_STRING:
			memcpy(out0_str, "str", 8);
			return 0;
		default:
			break;
	}

	fprintf(stderr, "Unknown argument type: %d\n", type);
	return -1;
}

int G00_CommandHelp(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* ref_config) {
	char command_name[255];
	char arg_type_str[8];
	unsigned int args_count = 0;
	G00_ConfigParseArgs(args, arg_defs, &args_count, &command_name);

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
