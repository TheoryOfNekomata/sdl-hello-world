#include "G00_config.h"

void G00_ConfigExecuteCommand(char command[255], char args[255], struct G00_Config* out0_config) {
	if (!strcmpi(command, "screen_size")) {
		//char width_str[8];
		//char height_str[8];

		sscanf(args, "%u %u", &out0_config->video.screen_width, &out0_config->video.screen_height);
		return;
	}
	if (!strcmpi(command, "ideal_fps")) {
		//char fps_str[8];

		sscanf(args, "%f", &out0_config->video.frames_per_second);
		out0_config->video.millis_per_tick = 1000.f / out0_config->video.frames_per_second;
		return;
	}

	fprintf(stderr, "Unknown command %s\n", command);
}

void G00_ConfigRead(const char* path, struct G00_Config* out0_config) {
	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		return;
	}

	char command_str[255];
	char args_str[255];
	char c;
	unsigned char i = 0;

	enum G00_ConfigParserMode mode = G00_CONFIG_PARSER_MODE_COMMAND;
	while (!feof(fp)) {
		c = fgetc(fp);

		switch (mode) {
			case G00_CONFIG_PARSER_MODE_COMMENT:
				if (c == '\r' || c == '\n') {
					mode = G00_CONFIG_PARSER_MODE_COMMAND;
					i = 0;
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
						G00_ConfigExecuteCommand(command_str, args_str, out0_config);
						i = 0;
						command_str[0] = '\0'; // reset command
						args_str[0] = '\0'; // reset args
					}
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
					G00_ConfigExecuteCommand(command_str, args_str, out0_config);
					mode = G00_CONFIG_PARSER_MODE_COMMAND;
					i = 0;
					command_str[0] = '\0';
					args_str[0] = '\0';
					continue;
				}
				args_str[i] = c;
				i += 1;
				break;
		}
	}
}
