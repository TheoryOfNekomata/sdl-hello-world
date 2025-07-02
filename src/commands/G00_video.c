#include "../G00_command.h"

int G00_CommandVideoScreenSize(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->video.screen_width, &out0_config->video.screen_height) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Screen size is %u x %u pixels\n", out0_config->video.screen_width, out0_config->video.screen_height);
		return 0;
	}

	if (!(0 < out0_config->video.screen_width && out0_config->video.screen_width < 99999u)) {
		out0_config->video.screen_width = 1024u;
		out0_config->video.screen_height = 768u;
		out0_config->video.aspect_ratio = (float) out0_config->video.screen_width / (float) out0_config->video.screen_height;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	if (!(0 < out0_config->video.screen_height && out0_config->video.screen_height < 99999u)) {
		out0_config->video.screen_width = 1024u;
		out0_config->video.screen_height = 768u;
		out0_config->video.aspect_ratio = (float) out0_config->video.screen_width / (float) out0_config->video.screen_height;

		G00_AppApplyConfig(out0_config->app);
		return 2;
	}

	out0_config->video.aspect_ratio = (float) out0_config->video.screen_width / (float) out0_config->video.screen_height;

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_CommandVideoIdealFps(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->video.frames_per_second) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Ideal frames-per-second value is %.3f\n", out0_config->video.frames_per_second);
		return 0;
	}

	if (out0_config->video.frames_per_second < 10.f) {
		out0_config->video.frames_per_second = 10.f;
		out0_config->video.millis_per_tick = 1000.f / out0_config->video.frames_per_second;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	if (out0_config->video.frames_per_second > 60.f) {
		out0_config->video.frames_per_second = 60.f;
		out0_config->video.millis_per_tick = 1000.f / out0_config->video.frames_per_second;

		G00_AppApplyConfig(out0_config->app);
		return 2;
	}

	out0_config->video.millis_per_tick = 1000.f / out0_config->video.frames_per_second;

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_CommandVideoMaxLoadedTextures(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->video.max_loaded_textures) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded textures count is %u\n", out0_config->video.max_loaded_textures);
		return 0;
	}

	if (!(16u <= out0_config->video.max_loaded_textures && out0_config->video.max_loaded_textures <= 255u)) {
		out0_config->video.max_loaded_textures = 16u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_CommandVideoMaxLoadedSprites(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->video.max_loaded_sprites) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded sprites count is %u\n", out0_config->video.max_loaded_sprites);
		return 0;
	}

	if (!(16u <= out0_config->video.max_loaded_sprites && out0_config->video.max_loaded_sprites <= 255u)) {
		out0_config->video.max_loaded_sprites = 16u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_CommandVideoMaxLoadedFonts(char args[255], struct G00_ConfigCommandArguments arg_defs, struct G00_Config* out0_config) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &out0_config->video.max_loaded_fonts) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded fonts is %u\n", out0_config->video.max_loaded_fonts);
		return 0;
	}

	if (!(8u <= out0_config->video.max_loaded_fonts && out0_config->video.max_loaded_fonts <= 255u)) {
		out0_config->video.max_loaded_fonts = 8u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}
