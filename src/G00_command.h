#ifndef G00_COMMAND_H
#define G00_COMMAND_H

#include "G00_config.h"
#include "G00_commands.h"

int G00_CommandParseArgs(char[255], struct G00_CommandArgumentDefinition, unsigned int* out0, ...);

static struct G00_ConfigCommandMappingEntry G00_CONFIG_COMMAND_MAPPING[] = {
	{
		.name = "help",
		.execute_fn = G00_XCommandHelp,
		.args = {
			.minimum = 0,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "command",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			},
		},
	},
	{
		.name = "video_screen_size",
		.execute_fn = G00_XCommandVideoScreenSize,
		.args = {
			.minimum = 2,
			.maximum = 2,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "width",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
				{
					.name = "height",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
			},
		}
	},
	{
		.name = "video_ideal_fps",
		.execute_fn = G00_XCommandVideoIdealFps,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "fps",
					.type = G00_COMMAND_ARGUMENT_TYPE_F32,
				},
			},
		}
	},
	{
		.name = "memory_pool_size_bytes",
		.execute_fn = G00_XCommandMemoryPoolSizeBytes,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U64,
				},
			},
		},
	},
	{
		.name = "memory_pool_max_entries",
		.execute_fn = G00_XCommandMemoryPoolMaxEntries,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U32,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_textures",
		.execute_fn = G00_XCommandVideoMaxLoadedTextures,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_fonts",
		.execute_fn = G00_XCommandVideoMaxLoadedFonts,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "video_max_loaded_sprites",
		.execute_fn = G00_XCommandVideoMaxLoadedSprites,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "bytes",
					.type = G00_COMMAND_ARGUMENT_TYPE_U8,
				},
			},
		},
	},
	{
		.name = "memory_asset_load",
		.execute_fn = G00_XCommandMemoryAssetLoad,
		.args = {
			.minimum = 1,
			.maximum = 1,
			.defs = (struct G00_CommandArgument[]) {
				{
					.name = "path",
					.type = G00_COMMAND_ARGUMENT_TYPE_STRING,
				},
			}
		}
	},

	{
		.name = "ui_msg_key",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_menu",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_item",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_title",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_description_text",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_label",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_menu_push",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_menu_pop",
		.execute_fn = NULL // TODO
	},
{
		.name = "ui_menu_show_start",
		.execute_fn = NULL // TODO
	},
	{
		.name = NULL,
		.execute_fn = NULL,
	},
};

#endif
