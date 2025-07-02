#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "G00_command.h"

static int parse_escaped_string(char *out, unsigned int maxlen, const char *start, const char **endptr) {
	const char *p = start;
	char quote = *p++;
	int len = 0;

	while (*p && *p != quote) {
		if (len >= maxlen - 1) return -1;

		if (*p == '\\') {
			p++;
			switch (*p) {
				case 'n': out[len++] = '\n'; break;
				case 't': out[len++] = '\t'; break;
				case 'r': out[len++] = '\r'; break;
				case '\\': out[len++] = '\\'; break;
				case '"': out[len++] = '"'; break;
				case '\'': out[len++] = '\''; break;
				case '0': out[len++] = '\0'; break;
				case '\0': return -1;
				default: out[len++] = *p; break;
			}
		} else {
			out[len++] = *p;
		}
		p++;
	}

	if (*p != quote) return -1;
	out[len] = '\0';
	*endptr = p + 1;
	return 0;
}

static int parse_unquoted_string(char *out, unsigned int maxlen, const char *start, const char **endptr) {
	const char *p = start;
	int len = 0;

	while (*p && !isspace((unsigned char)*p)) {
		if (len >= maxlen - 1) return -1;

		if (*p == '\\') {
			p++;
			switch (*p) {
				case 'n': out[len++] = '\n'; break;
				case 't': out[len++] = '\t'; break;
				case 'r': out[len++] = '\r'; break;
				case '\\': out[len++] = '\\'; break;
				case '"': out[len++] = '"'; break;
				case '\'': out[len++] = '\''; break;
				case '0': out[len++] = '\0'; break;
				case '\0': return -1;
				default: out[len++] = *p; break;
			}
		} else {
			out[len++] = *p;
		}
		p++;
	}

	out[len] = '\0';
	*endptr = p;
	return 0;
}

int G00_CommandParseArgs(char input[255], struct G00_CommandArgumentDefinition arg_defs, unsigned int* out0_parsed_args, ...) {
	char buffer[255];
	strncpy(buffer, input, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	char *p = buffer;
	va_list args;
	va_start(args, out0_parsed_args);

	unsigned char parsed_args = 0;
	for (size_t i = 0; ; i += 1) {
		while (isspace((unsigned char)*p)) {
			p += 1;
		}

		if (*p == '\0') {
			if (parsed_args == 0) {
				if (out0_parsed_args != NULL) {
					*out0_parsed_args = parsed_args;
				}
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

		if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_U32) {
			unsigned int* out = va_arg(args, unsigned int*);
			unsigned int val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u32 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = val;
		} else if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_U8) {
			unsigned char* out = va_arg(args, unsigned char*);
			unsigned long val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u8 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = (unsigned char) val;
		} else if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_U16) {
			unsigned short* out = va_arg(args, unsigned short*);
			unsigned long val = strtoul(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u16 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = (unsigned short) val;
		} else if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_U64) {
			unsigned long long* out = va_arg(args, unsigned long long*);
			unsigned long long val = strtoull(p, &endptr, 10);
			if (endptr == p || errno != 0) {
				fprintf(stderr, "Failed to parse u64 at position %zu\n", i);
				va_end(args);
				return -2;
			}
			*out = val;
		} else if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_STRING) {
			char *dest = va_arg(args, char*);
			unsigned int maxlen = 255;

			int result;
			if (*p == '"' || *p == '\'') {
				result = parse_escaped_string(dest, maxlen, p, &endptr);
			} else {
				result = parse_unquoted_string(dest, maxlen, p, &endptr);
			}

			if (result != 0) {
				fprintf(stderr, "Failed to parse string at position %zu\n", i);
				va_end(args);
				return -2;
			}
		} else if (arg_defs.defs[i].type == G00_COMMAND_ARGUMENT_TYPE_F32) {
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

	if (out0_parsed_args != NULL) {
		*out0_parsed_args = parsed_args;
	}
	va_end(args);
	return 0;
}
