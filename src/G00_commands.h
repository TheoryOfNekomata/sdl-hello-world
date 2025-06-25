#ifndef G00_COMMANDS_H
#define G00_COMMANDS_H

#include "G00_config.h"

G00_ConfigCommandFunction G00_ConfigCommandSetScreenSize;
G00_ConfigCommandFunction G00_ConfigCommandSetIdealFps;
G00_ConfigCommandFunction G00_ConfigCommandSetMemoryPoolSize;
G00_ConfigCommandFunction G00_ConfigCommandSetMemoryMaxEntries;
G00_ConfigCommandFunction G00_ConfigCommandSetVideoMaxLoadedTextures;
G00_ConfigCommandFunction G00_ConfigCommandSetVideoMaxLoadedFonts;
G00_ConfigCommandFunction G00_ConfigCommandSetVideoMaxLoadedSprites;

static struct G00_ConfigCommandMappingEntry G00_CONFIG_COMMAND_MAPPING[] = {
	{
		.name = "screen_size",
		.fn = G00_ConfigCommandSetScreenSize,
	},
	{
		.name = "ideal_fps",
		.fn = G00_ConfigCommandSetIdealFps,
	},
	{
		.name = "memory_pool_size_bytes",
		.fn = G00_ConfigCommandSetMemoryPoolSize,
	},
	{
		.name = "memory_pool_max_entries",
		.fn = G00_ConfigCommandSetMemoryMaxEntries,
	},
	{
		.name = "video_max_loaded_textures",
		.fn = G00_ConfigCommandSetVideoMaxLoadedTextures,
	},
	{
		.name = "video_max_loaded_fonts",
		.fn = G00_ConfigCommandSetVideoMaxLoadedFonts,
	},
	{
		.name = "video_max_loaded_sprites",
		.fn = G00_ConfigCommandSetVideoMaxLoadedSprites,
	},

	{
		.name = "msg_key",
		.fn = NULL // TODO
	},
{
		.name = "menu",
		.fn = NULL // TODO
	},
{
		.name = "item",
		.fn = NULL // TODO
	},
{
		.name = "title",
		.fn = NULL // TODO
	},
{
		.name = "description_text",
		.fn = NULL // TODO
	},
{
		.name = "label",
		.fn = NULL // TODO
	},
{
		.name = "menu_push",
		.fn = NULL // TODO
	},
{
		.name = "menu_pop",
		.fn = NULL // TODO
	},
{
		.name = "menu_show_start",
		.fn = NULL // TODO
	},

	{
		.name = NULL,
		.fn = NULL,
	},
};

#endif
