#include "../G00_video.h"
#include "../G00_command.h"

int G00_CommandVideoScreenSize(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_Video* video_state) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &video_state->config.screen_width, &video_state->config.screen_height) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Screen size is %u x %u pixels\n", video_state->config.screen_width, video_state->config.screen_height);
		return 0;
	}

	if (!(0 < video_state->config.screen_width && video_state->config.screen_width < 99999u)) {
		video_state->config.screen_width = 1024u;
		video_state->config.screen_height = 768u;
		video_state->config.aspect_ratio = (float) video_state->config.screen_width / (float) video_state->config.screen_height;
		G00_VideoUpdateWindow(video_state);
		return 1;
	}

	if (!(0 < video_state->config.screen_height && video_state->config.screen_height < 99999u)) {
		video_state->config.screen_width = 1024u;
		video_state->config.screen_height = 768u;
		video_state->config.aspect_ratio = (float) video_state->config.screen_width / (float) video_state->config.screen_height;
		G00_VideoUpdateWindow(video_state);
		return 2;
	}

	video_state->config.aspect_ratio = (float) video_state->config.screen_width / (float) video_state->config.screen_height;
	G00_VideoUpdateWindow(video_state);
	return 0;
}

int G00_CommandVideoIdealFps(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_Video* video_state) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &video_state->config.frames_per_second) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Ideal frames-per-second value is %.3f\n", video_state->config.frames_per_second);
		return 0;
	}

	if (video_state->config.frames_per_second < 10.f) {
		video_state->config.frames_per_second = 10.f;
		video_state->config.millis_per_tick = 1000.f / video_state->config.frames_per_second;
		return 1;
	}

	if (video_state->config.frames_per_second > 60.f) {
		video_state->config.frames_per_second = 60.f;
		video_state->config.millis_per_tick = 1000.f / video_state->config.frames_per_second;
		return 2;
	}

	video_state->config.millis_per_tick = 1000.f / video_state->config.frames_per_second;
	return 0;
}

int G00_CommandVideoMaxLoadedTextures(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_Video* video_state) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &video_state->config.max_loaded_textures) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded textures count is %u\n", video_state->config.max_loaded_textures);
		return 0;
	}

	if (!(16u <= video_state->config.max_loaded_textures && video_state->config.max_loaded_textures <= 255u)) {
		video_state->config.max_loaded_textures = 16u;
		// TODO update array
		return 1;
	}

	// TODO update array
	return 0;
}

int G00_CommandVideoMaxLoadedSprites(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_Video* video_state) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &video_state->config.max_loaded_sprites) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded sprites count is %u\n", video_state->config.max_loaded_sprites);
		return 0;
	}

	if (!(16u <= video_state->config.max_loaded_sprites && video_state->config.max_loaded_sprites <= 255u)) {
		video_state->config.max_loaded_sprites = 16u;
		// TODO update array
		return 1;
	}

	// TODO update array
	return 0;
}

int G00_CommandVideoMaxLoadedFonts(char args[255], struct G00_CommandArgumentDefinition arg_defs, struct G00_Video* video_state) {
	unsigned int args_count = 0;
	if (G00_ConfigParseArgs(args, arg_defs, &args_count, &video_state->config.max_loaded_fonts) < 0) {
		return -1;
	}

	if (args_count == 0) {
		fprintf(stdout, "Video maximum loaded fonts is %u\n", video_state->config.max_loaded_fonts);
		return 0;
	}

	if (!(8u <= video_state->config.max_loaded_fonts && video_state->config.max_loaded_fonts <= 255u)) {
		video_state->config.max_loaded_fonts = 8u;

		// TODO update array
		return 1;
	}

	// TODO update array
	return 0;
}
