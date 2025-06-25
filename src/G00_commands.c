#include "G00_commands.h"

int G00_AppApplyConfig(struct G00_App* app);

int G00_ConfigCommandSetScreenSize(char args[255], struct G00_Config* out0_config) {
	//char width_str[8];
	//char height_str[8];
	sscanf(args, "%u %u", &out0_config->video.screen_width, &out0_config->video.screen_height);
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

int G00_ConfigCommandSetIdealFps(char args[255], struct G00_Config* out0_config) {
	//char fps_str[8];
	sscanf(args, "%f", &out0_config->video.frames_per_second);

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

int G00_ConfigCommandSetMemoryPoolSize(char args[255], struct G00_Config* out0_config) {
	sscanf(args, "%llu", &out0_config->memory.pool_size_bytes);

	if (!(10485760u <= out0_config->memory.pool_size_bytes && out0_config->memory.pool_size_bytes <= 2147483648u)) {
		out0_config->memory.pool_size_bytes = 10485760u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_ConfigCommandSetMemoryMaxEntries(char args[255], struct G00_Config* out0_config) {
	sscanf(args, "%u", &out0_config->memory.pool_max_entries);

	if (!(256u <= out0_config->memory.pool_max_entries && out0_config->memory.pool_max_entries <= 131072u)) {
		out0_config->memory.pool_max_entries = 256u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_ConfigCommandSetVideoMaxLoadedTextures(char args[255], struct G00_Config* out0_config) {
	sscanf(args, "%u", &out0_config->video.max_loaded_textures);

	if (!(16u <= out0_config->video.max_loaded_textures && out0_config->video.max_loaded_textures <= 255u)) {
		out0_config->video.max_loaded_textures = 16u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_ConfigCommandSetVideoMaxLoadedSprites(char args[255], struct G00_Config* out0_config) {
	sscanf(args, "%u", &out0_config->video.max_loaded_sprites);

	if (!(16u <= out0_config->video.max_loaded_sprites && out0_config->video.max_loaded_sprites <= 255u)) {
		out0_config->video.max_loaded_sprites = 16u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}

int G00_ConfigCommandSetVideoMaxLoadedFonts(char args[255], struct G00_Config* out0_config) {
	sscanf(args, "%u", &out0_config->video.max_loaded_fonts);

	if (!(8u <= out0_config->video.max_loaded_fonts && out0_config->video.max_loaded_fonts <= 255u)) {
		out0_config->video.max_loaded_fonts = 8u;

		G00_AppApplyConfig(out0_config->app);
		return 1;
	}

	G00_AppApplyConfig(out0_config->app);
	return 0;
}
