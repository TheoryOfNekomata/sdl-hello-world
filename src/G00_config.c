#include <stdio.h>

#include "G00_config.h"
#include "G00_command.h"
#include "G00_commands.h"

int G00_ConfigExecuteCommand(const char* path, unsigned int line, char command[255], char args[255], struct G00_App* app) {
	int result = G00_RuntimeExecuteCommand(G00_COMMAND_ENTRIES, command, args, app);
	if (result < 0) {
		fprintf(stderr, "%s:%u - ", path, line);
	}
	if (result > 0) {
		fprintf(stdout, "%s:%u - ", path, line);
	}

	return result;
}

enum G00_ConfigParserMode : unsigned char {
	G00_CONFIG_PARSER_MODE_COMMAND,
	G00_CONFIG_PARSER_MODE_ARGS,
	G00_CONFIG_PARSER_MODE_COMMENT,
	G00_CONFIG_PARSER_MODE_ESCAPE,
};

enum G00_ConfigCommandType : unsigned char {
	G00_CONFIG_COMMAND_TYPE_UNKNOWN,
	G00_CONFIG_COMMAND_TYPE_SCREEN_SIZE,
	G00_CONFIG_COMMAND_TYPE_IDEAL_FPS,
};

void G00_ConfigExecuteScript(const char* path, struct G00_App* app) {
	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		return;
	}

	char command_str[255];
	char args_str[255];
	char c;
	unsigned char i = 0;
	unsigned int current_line = 1;

	enum G00_ConfigParserMode mode = G00_CONFIG_PARSER_MODE_COMMAND;
	while (!feof(fp)) {
		c = fgetc(fp);

		switch (mode) {
			case G00_CONFIG_PARSER_MODE_COMMENT:
				if (c == '\r' || c == '\n') {
					mode = G00_CONFIG_PARSER_MODE_COMMAND;
					i = 0;
					current_line += 1;
				}
				break;
			case G00_CONFIG_PARSER_MODE_COMMAND:
				if (c == '\\') {
					mode = G00_CONFIG_PARSER_MODE_ESCAPE;
					continue;
				}
				if (c == '#') {
					mode = G00_CONFIG_PARSER_MODE_COMMENT;
					continue;
				}
				if (c == '\n' || c == '\r') {
					if (i > 0) {
						command_str[i] = '\0';
						G00_ConfigExecuteCommand(path, current_line, command_str, args_str, app);
						i = 0;
						command_str[0] = '\0'; // reset command
						args_str[0] = '\0'; // reset args
					}
					current_line += 1;
					continue;
				}
				if (c == ' ' || c == '\t') {
					if (i == 0) {
						continue;
					}

					command_str[i] = '\0';
					i = 0;
					mode = G00_CONFIG_PARSER_MODE_ARGS;
					break;
				}
				command_str[i] = c;
				i += 1;
				break;
			case G00_CONFIG_PARSER_MODE_ESCAPE:
				if (c == '\r') {
					current_line += 1;
					continue;
				}
				args_str[i] = c;
				i += 1;
				break;
			case G00_CONFIG_PARSER_MODE_ARGS:
				if (c == '\\') {
					mode = G00_CONFIG_PARSER_MODE_ESCAPE;
					continue;
				}
				if (c == '#') {
					mode = G00_CONFIG_PARSER_MODE_COMMENT;
					continue;
				}
				if (c == '\n' || c == '\r') {
					args_str[i] = '\0';
					G00_ConfigExecuteCommand(path, current_line, command_str, args_str, app);
					mode = G00_CONFIG_PARSER_MODE_COMMAND;
					i = 0;
					command_str[0] = '\0';
					args_str[0] = '\0';
					current_line += 1;
					continue;
				}
				args_str[i] = c;
				i += 1;
				break;
		}
	}

	fclose(fp);
}
